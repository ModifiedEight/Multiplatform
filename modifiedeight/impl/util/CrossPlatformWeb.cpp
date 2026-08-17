#include <util/CrossPlatformWeb.hpp>
#include <cstdlib>

#ifdef ANDROID
#include <jni.h>
#include <android/JVMAttacher.hpp>
#include <android/AppPlatform_android.hpp>

static JNIEnv* getEnv() {
    AppPlatform_android* platform = (AppPlatform_android*)AppPlatform::_singleton;
    if(!platform) return nullptr;
    JNIEnv* env;
    platform->jvm->AttachCurrentThread(&env, NULL);
    return env;
}

static std::string encodeUrlSpaces(const std::string& url) {
    std::string out = url;
    size_t pos = 0;
    while((pos = out.find(" ", pos)) != std::string::npos) {
        out.replace(pos, 1, "%20");
        pos += 3;
    }
    return out;
}

std::vector<unsigned char> CrossPlatform_DownloadBinary(const std::string& rawUrl) {
    std::string url = encodeUrlSpaces(rawUrl);
    JNIEnv* env = getEnv();
    if(!env) return {};
    
    jclass urlClass = env->FindClass("java/net/URL");
    jmethodID urlInit = env->GetMethodID(urlClass, "<init>", "(Ljava/lang/String;)V");
    jstring jUrl = env->NewStringUTF(url.c_str());
    jobject urlObj = env->NewObject(urlClass, urlInit, jUrl);
    env->DeleteLocalRef(jUrl);
    
    jmethodID openConn = env->GetMethodID(urlClass, "openConnection", "()Ljava/net/URLConnection;");
    jobject connObj = env->CallObjectMethod(urlObj, openConn);
    env->DeleteLocalRef(urlObj);
    if(env->ExceptionCheck()) { env->ExceptionClear(); return {}; }
    
    jclass connClass = env->GetObjectClass(connObj);
    jmethodID getInputStream = env->GetMethodID(connClass, "getInputStream", "()Ljava/io/InputStream;");
    jobject isObj = env->CallObjectMethod(connObj, getInputStream);
    env->DeleteLocalRef(connClass);
    if(env->ExceptionCheck()) { env->ExceptionClear(); return {}; }
    
    jclass isClass = env->FindClass("java/io/InputStream");
    jmethodID read = env->GetMethodID(isClass, "read", "([B)I");
    
    jbyteArray buffer = env->NewByteArray(8192);
    std::vector<unsigned char> data;
    while(true) {
        jint bytesRead = env->CallIntMethod(isObj, read, buffer);
        if(env->ExceptionCheck() || bytesRead <= 0) {
            env->ExceptionClear();
            break;
        }
        jbyte* elems = env->GetByteArrayElements(buffer, NULL);
        data.insert(data.end(), elems, elems + bytesRead);
        env->ReleaseByteArrayElements(buffer, elems, JNI_ABORT);
    }
    env->DeleteLocalRef(buffer);
    
    jmethodID close = env->GetMethodID(isClass, "close", "()V");
    env->CallVoidMethod(isObj, close);
    env->DeleteLocalRef(isObj);
    env->DeleteLocalRef(isClass);
    return data;
}

std::string CrossPlatform_DownloadString(const std::string& url) {
    std::vector<unsigned char> data = CrossPlatform_DownloadBinary(url);
    if(data.empty()) return "";
    return std::string(data.begin(), data.end());
}

bool CrossPlatform_ExtractZip(const std::string& zipFile, const std::string& outDir) {
    JNIEnv* env = getEnv();
    if(!env) return false;
    
    jclass fileClass = env->FindClass("java/io/File");
    jclass fisClass = env->FindClass("java/io/FileInputStream");
    jclass zisClass = env->FindClass("java/util/zip/ZipInputStream");
    jclass zeClass = env->FindClass("java/util/zip/ZipEntry");
    jclass fosClass = env->FindClass("java/io/FileOutputStream");
    
    jmethodID fileInit = env->GetMethodID(fileClass, "<init>", "(Ljava/lang/String;)V");
    jmethodID fileMkdirs = env->GetMethodID(fileClass, "mkdirs", "()Z");
    jmethodID fileGetParentFile = env->GetMethodID(fileClass, "getParentFile", "()Ljava/io/File;");
    
    jmethodID fisInit = env->GetMethodID(fisClass, "<init>", "(Ljava/lang/String;)V");
    jmethodID zisInit = env->GetMethodID(zisClass, "<init>", "(Ljava/io/InputStream;)V");
    jmethodID getNextEntry = env->GetMethodID(zisClass, "getNextEntry", "()Ljava/util/zip/ZipEntry;");
    jmethodID isDirectory = env->GetMethodID(zeClass, "isDirectory", "()Z");
    jmethodID getName = env->GetMethodID(zeClass, "getName", "()Ljava/lang/String;");
    jmethodID zisRead = env->GetMethodID(zisClass, "read", "([B)I");
    jmethodID zisCloseEntry = env->GetMethodID(zisClass, "closeEntry", "()V");
    jmethodID zisClose = env->GetMethodID(zisClass, "close", "()V");
    
    jmethodID fosInit = env->GetMethodID(fosClass, "<init>", "(Ljava/lang/String;)V");
    jmethodID fosWrite = env->GetMethodID(fosClass, "write", "([BII)V");
    jmethodID fosClose = env->GetMethodID(fosClass, "close", "()V");
    
    jstring jZipFile = env->NewStringUTF(zipFile.c_str());
    jobject fisObj = env->NewObject(fisClass, fisInit, jZipFile);
    if(env->ExceptionCheck()) { env->ExceptionClear(); return false; }
    
    jobject zisObj = env->NewObject(zisClass, zisInit, fisObj);
    jbyteArray buffer = env->NewByteArray(8192);
    
    while(true) {
        if (env->PushLocalFrame(64) < 0) {
            break;
        }
        jobject zeObj = env->CallObjectMethod(zisObj, getNextEntry);
        if(env->ExceptionCheck() || !zeObj) {
            env->ExceptionClear();
            env->PopLocalFrame(NULL);
            break;
        }

        jstring jName = (jstring)env->CallObjectMethod(zeObj, getName);
        if (jName) {
            const char* nameStr = env->GetStringUTFChars(jName, 0);
            if (nameStr) {
                std::string fullPath = outDir + "/" + nameStr;
                env->ReleaseStringUTFChars(jName, nameStr);

                jboolean isDir = env->CallBooleanMethod(zeObj, isDirectory);
                if(isDir) {
                    jstring jPath = env->NewStringUTF(fullPath.c_str());
                    jobject dirFile = env->NewObject(fileClass, fileInit, jPath);
                    if (dirFile) {
                        env->CallBooleanMethod(dirFile, fileMkdirs);
                    }
                } else {
                    jstring jPath = env->NewStringUTF(fullPath.c_str());
                    jobject outFile = env->NewObject(fileClass, fileInit, jPath);
                    if (outFile) {
                        jobject parentFile = env->CallObjectMethod(outFile, fileGetParentFile);
                        if(parentFile) {
                            env->CallBooleanMethod(parentFile, fileMkdirs);
                        }

                        jobject fosObj = env->NewObject(fosClass, fosInit, jPath);
                        if(!env->ExceptionCheck() && fosObj) {
                            while(true) {
                                jint bytesRead = env->CallIntMethod(zisObj, zisRead, buffer);
                                if(bytesRead <= 0) break;
                                env->CallVoidMethod(fosObj, fosWrite, buffer, 0, bytesRead);
                            }
                            env->CallVoidMethod(fosObj, fosClose);
                        } else {
                            env->ExceptionClear();
                        }
                    }
                }
            }
        }
        env->CallVoidMethod(zisObj, zisCloseEntry);
        env->PopLocalFrame(NULL);
    }

    env->CallVoidMethod(zisObj, zisClose);
    env->DeleteLocalRef(buffer);
    env->DeleteLocalRef(zisObj);
    env->DeleteLocalRef(fisObj);

    return true;
}

#elif defined(WIN32) || defined(_WIN32)
#include <winsock2.h>
#include <windows.h>

typedef PVOID HINTERNET;
typedef HINTERNET (WINAPI *InternetOpenA_t)(LPCSTR, DWORD, LPCSTR, LPCSTR, DWORD);
typedef HINTERNET (WINAPI *InternetOpenUrlA_t)(HINTERNET, LPCSTR, LPCSTR, DWORD, DWORD, DWORD_PTR);
typedef BOOL (WINAPI *InternetReadFile_t)(HINTERNET, LPVOID, DWORD, LPDWORD);
typedef BOOL (WINAPI *InternetCloseHandle_t)(HINTERNET);

static std::string encodeUrlSpaces(const std::string& url) {
    std::string out = url;
    size_t pos = 0;
    while((pos = out.find(" ", pos)) != std::string::npos) {
        out.replace(pos, 1, "%20");
        pos += 3;
    }
    return out;
}

std::vector<unsigned char> CrossPlatform_DownloadBinary(const std::string& rawUrl) {
    std::string url = encodeUrlSpaces(rawUrl);
    HMODULE hWinInet = LoadLibraryA("wininet.dll");
    if(!hWinInet) return {};
    
    InternetOpenA_t pInternetOpenA = (InternetOpenA_t)GetProcAddress(hWinInet, "InternetOpenA");
    InternetOpenUrlA_t pInternetOpenUrlA = (InternetOpenUrlA_t)GetProcAddress(hWinInet, "InternetOpenUrlA");
    InternetReadFile_t pInternetReadFile = (InternetReadFile_t)GetProcAddress(hWinInet, "InternetReadFile");
    InternetCloseHandle_t pInternetCloseHandle = (InternetCloseHandle_t)GetProcAddress(hWinInet, "InternetCloseHandle");
    
    if(!pInternetOpenA || !pInternetOpenUrlA || !pInternetReadFile || !pInternetCloseHandle) return {};
    
    HINTERNET hInternet = pInternetOpenA("MinecraftPE_DL", 0, NULL, NULL, 0); // INTERNET_OPEN_TYPE_PRECONFIG = 0
    if(!hInternet) return {};
    HINTERNET hUrl = pInternetOpenUrlA(hInternet, url.c_str(), NULL, 0, 0x80000000 | 0x04000000, 0); // INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE
    if(!hUrl) { pInternetCloseHandle(hInternet); return {}; }
    
    std::vector<unsigned char> data;
    char buffer[8192];
    DWORD bytesRead;
    while(pInternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        data.insert(data.end(), buffer, buffer + bytesRead);
    }
    pInternetCloseHandle(hUrl);
    pInternetCloseHandle(hInternet);
    return data;
}

std::string CrossPlatform_DownloadString(const std::string& url) {
    std::vector<unsigned char> data = CrossPlatform_DownloadBinary(url);
    if(data.empty()) return "";
    return std::string(data.begin(), data.end());
}

bool CrossPlatform_ExtractZip(const std::string& zipFile, const std::string& outDir) {
    std::string cmd = "powershell -noprofile -command \"Expand-Archive -Force '";
    cmd += zipFile + "' '" + outDir + "'\"";
    int res = system(cmd.c_str());
    return res == 0;
}

#elif defined(__linux__) && !defined(ANDROID)
// Linux / Mac
#include <curl/curl.h>

struct CurlBuf {
  std::vector<unsigned char> data;
};
static size_t curlWriteCB(void *ptr, size_t sz, size_t n, void *ud) {
  auto *b = static_cast<CurlBuf *>(ud);
  const unsigned char *p = static_cast<const unsigned char *>(ptr);
  b->data.insert(b->data.end(), p, p + sz * n);
  return sz * n;
}

static std::string encodeUrlSpaces(const std::string& url) {
    std::string out = url;
    size_t pos = 0;
    while((pos = out.find(" ", pos)) != std::string::npos) {
        out.replace(pos, 1, "%20");
        pos += 3;
    }
    return out;
}

std::vector<unsigned char> CrossPlatform_DownloadBinary(const std::string& rawUrl) {
  std::string url = encodeUrlSpaces(rawUrl);
  CurlBuf buf;
  CURL *c = curl_easy_init();
  if (!c) return {};
  curl_easy_setopt(c, CURLOPT_URL, url.c_str());
  curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, curlWriteCB);
  curl_easy_setopt(c, CURLOPT_WRITEDATA, &buf);
  curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(c, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(c, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
  CURLcode res = curl_easy_perform(c);
  long http_code = 0;
  curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &http_code);
  curl_easy_cleanup(c);
  if (res != CURLE_OK || (http_code >= 400 && http_code < 600)) {
      return {};
  }
  return buf.data;
}

std::string CrossPlatform_DownloadString(const std::string& url) {
    std::vector<unsigned char> data = CrossPlatform_DownloadBinary(url);
    if(data.empty()) return "";
    return std::string(data.begin(), data.end());
}

bool CrossPlatform_ExtractZip(const std::string& zipFile, const std::string& outDir) {
    std::string cmd = "mkdir -p \"" + outDir + "\" && unzip -o \"" + zipFile + "\" -d \"" + outDir + "\" >/dev/null 2>&1";
    int res = system(cmd.c_str());
    int exit_code = 0;
#ifdef WEXITSTATUS
    exit_code = WEXITSTATUS(res);
#else
    exit_code = (res >> 8) & 0xff;
#endif
    return exit_code == 0 || exit_code == 1;
}
#endif
