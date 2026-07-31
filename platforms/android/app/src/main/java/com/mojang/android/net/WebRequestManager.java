package com.mojang.android.net;

import android.util.Log;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.security.InvalidParameterException;
import java.util.ArrayList;
import java.util.Iterator;
import org.apache.http.Header;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpVersion;
import org.apache.http.ParseException;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpDelete;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpPut;
import org.apache.http.client.methods.HttpRequestBase;
import org.apache.http.conn.ClientConnectionManager;
import org.apache.http.conn.scheme.PlainSocketFactory;
import org.apache.http.conn.scheme.Scheme;
import org.apache.http.conn.scheme.SchemeRegistry;
import org.apache.http.conn.ssl.SSLSocketFactory;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.impl.conn.tsccm.ThreadSafeClientConnManager;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;
import org.apache.http.params.HttpProtocolParams;
import org.apache.http.util.EntityUtils;

/* loaded from: classes.dex */
public class WebRequestManager {
    private IRequestCompleteCallback onRequestCompleteCallback;
    private HttpClient _httpClient = null;
    private ArrayList<WebRequestData> _webRequests = new ArrayList<>();
    private Object _requestlock = new Object();

    public interface IRequestCompleteCallback {
        void onRequestComplete(int i, long j, int i2, String str);
    }

    public WebRequestManager(IRequestCompleteCallback onRequestCompleteCallback) {
        this.onRequestCompleteCallback = onRequestCompleteCallback;
    }

    private void _init() {
        HttpParams params = new BasicHttpParams();
        HttpProtocolParams.setVersion(params, HttpVersion.HTTP_1_1);
        HttpProtocolParams.setContentCharset(params, "utf-8");
        params.setBooleanParameter("http.protocol.expect-continue", false);
        SchemeRegistry registry = new SchemeRegistry();
        registry.register(new Scheme("http", PlainSocketFactory.getSocketFactory(), 80));
        try {
            SSLSocketFactory sslSocketFactory = NoCertSSLSocketFactory.createDefault();
            registry.register(new Scheme("https", sslSocketFactory, 443));
            ClientConnectionManager manager = new ThreadSafeClientConnManager(params, registry);
            this._httpClient = new DefaultHttpClient(manager, params);
        } catch (Exception e) {
            Log.e("MCPE_ssl", "Couldn't create SSLSocketFactory");
        }
    }

    public void webRequest(int requestId, long voidptr, String uri, String method, String cookieData, String httpBody) {
        HttpRequestBase httpRequest;
        if (this._httpClient == null) {
            _init();
        }
        if (method.equals("DELETE")) {
            httpRequest = new HttpDelete(uri);
        } else if (method.equals("PUT")) {
            HttpPut putRequest = new HttpPut(uri);
            if (httpBody != "") {
                try {
                    StringEntity se = new StringEntity(httpBody);
                    se.setContentType("application/json");
                    putRequest.setEntity(se);
                } catch (UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
            }
            httpRequest = putRequest;
        } else if (method.equals("GET")) {
            httpRequest = new HttpGet(uri);
        } else if (method.equals("POST")) {
            HttpPost postRequest = new HttpPost(uri);
            if (httpBody != "") {
                try {
                    StringEntity se2 = new StringEntity(httpBody);
                    se2.setContentType("application/json");
                    postRequest.setEntity(se2);
                } catch (UnsupportedEncodingException e2) {
                    e2.printStackTrace();
                }
            }
            httpRequest = postRequest;
        } else {
            throw new InvalidParameterException("Unknown request method " + method);
        }
        httpRequest.addHeader("User-Agent", "MCPE/Curl");
        HttpParams httpParameters = new BasicHttpParams();
        HttpConnectionParams.setConnectionTimeout(httpParameters, 30000);
        httpRequest.setParams(httpParameters);
        if (cookieData != null && cookieData.length() > 0) {
            System.out.println("Setting cookie: (" + cookieData.length() + ") " + cookieData);
            httpRequest.addHeader("Cookie", cookieData);
        }
        final WebRequestData request = new WebRequestData(requestId, httpRequest, voidptr);
        synchronized (this._requestlock) {
            Iterator i$ = this._webRequests.iterator();
            while (i$.hasNext()) {
                WebRequestData w = (WebRequestData) i$.next();
                if (w.requestId == request.requestId) {
                    return;
                }
            }
            this._webRequests.add(request);
            new Thread(new Runnable() { // from class: com.mojang.android.net.WebRequestManager.1
                @Override // java.lang.Runnable
                public void run() {
                    do {
                        try {
                            request.execute(WebRequestManager.this._httpClient);
                            if (request.getStatusCode() != 503) {
                                break;
                            } else {
                                request.waitForSeconds(request.retryTimeout);
                            }
                        } catch (Exception e3) {
                            e3.printStackTrace();
                            request.markError(Status.FAIL_TIMEOUT);
                        }
                    } while (!request.aborted);
                    synchronized (WebRequestManager.this._requestlock) {
                        if (!request.aborted) {
                            WebRequestManager.this.onRequestCompleteCallback.onRequestComplete(request.requestId, request.voidptr, request.getStatusCode(), request.content);
                        }
                    }
                }
            }).start();
        }
    }

    public int getWebRequestStatus(int requestId) {
        return _findWebRequest(requestId).getStatusCode();
    }

    public String getWebRequestContent(int requestId) {
        return _findWebRequest(requestId).content;
    }

    public int abortWebRequest(int requestId) {
        WebRequestData r = _findWebRequest(requestId);
        if (r.status != Status.REQUEST_NOT_FOUND) {
            synchronized (this._requestlock) {
                r.abort();
                this._webRequests.remove(r);
            }
            System.out.println("Requests left " + this._webRequests.size());
        }
        return r.getStatusCode();
    }

    private WebRequestData _findWebRequest(int requestId) {
        synchronized (this._requestlock) {
            Iterator i$ = this._webRequests.iterator();
            while (i$.hasNext()) {
                WebRequestData r = (WebRequestData) i$.next();
                if (r.requestId == requestId) {
                    return r;
                }
            }
            return new WebRequestData(requestId).markError(Status.REQUEST_NOT_FOUND);
        }
    }

    public enum Status {
        REQUEST_NOT_FOUND,
        PENDING,
        FINISHED,
        FAIL_URLFORMAT,
        FAIL_PARSE,
        FAIL_TIMEOUT,
        FAIL_GENERAL,
        FAIL_CANCELLED;

        public boolean isError() {
            return (this == PENDING || this == FINISHED) ? false : true;
        }

        public int getCode() {
            return -ordinal();
        }
    }

    private static class WebRequestData {
        static final /* synthetic */ boolean $assertionsDisabled;
        private HttpRequestBase request;
        public final int requestId;
        private HttpResponse response;
        public long voidptr;
        public Status status = Status.PENDING;
        public String content = "";
        public volatile boolean aborted = false;
        public int retryTimeout = 5;

        static {
            $assertionsDisabled = !WebRequestManager.class.desiredAssertionStatus();
        }

        public WebRequestData(int requestId) {
            this.requestId = requestId;
        }

        public WebRequestData(int requestId, HttpRequestBase request, long voidptr) {
            this.requestId = requestId;
            this.request = request;
            this.voidptr = voidptr;
        }

        public void waitForSeconds(int seconds) throws InterruptedException {
            for (int a = 0; a < seconds * 5 && !this.aborted; a++) {
                try {
                    Thread.sleep(a * 200);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

        public WebRequestData markError(Status errorCode) {
            if (!$assertionsDisabled && !errorCode.isError()) {
                throw new AssertionError();
            }
            this.status = errorCode;
            return this;
        }

        public void abort() {
            this.aborted = true;
            if (this.request != null && !this.request.isAborted()) {
                this.request.abort();
            }
        }

        public void execute(HttpClient httpClient) {
            if (Status.PENDING == this.status) {
                try {
                    this.response = httpClient.execute(this.request);
                } catch (ClientProtocolException e1) {
                    markError(Status.FAIL_URLFORMAT);
                    e1.printStackTrace();
                } catch (IOException e12) {
                    markError(Status.FAIL_GENERAL);
                    e12.printStackTrace();
                } catch (Exception e13) {
                    markError(Status.FAIL_GENERAL);
                    e13.printStackTrace();
                }
                if (this.response != null) {
                    try {
                        int responseCode = this.response.getStatusLine().getStatusCode();
                        if (responseCode == 204) {
                            this.content = "";
                        } else if (responseCode == 503) {
                            Header h = this.response.getLastHeader("Retry-After");
                            if (h != null) {
                                try {
                                    this.retryTimeout = Integer.valueOf(h.getValue()).intValue();
                                } catch (Exception e) {
                                }
                            }
                        } else {
                            HttpEntity entity = this.response.getEntity();
                            this.content = EntityUtils.toString(entity);
                        }
                        this.status = Status.FINISHED;
                    } catch (IOException e2) {
                        e2.printStackTrace();
                        this.status = Status.FAIL_GENERAL;
                    } catch (ParseException e3) {
                        e3.printStackTrace();
                        this.status = Status.FAIL_PARSE;
                    }
                }
            }
        }

        public int getStatusCode() {
            return Status.FINISHED == this.status ? this.response.getStatusLine().getStatusCode() : this.status.getCode();
        }
    }
}
