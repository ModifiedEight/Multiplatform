package com.mojang.minecraftpe;

import android.content.Context;
import android.text.InputFilter;
import android.text.Spanned;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputConnectionWrapper;
import android.widget.EditText;

/* loaded from: classes.dex */
public class TextInputProxyEditTextbox extends EditText {
    private MCPEKeyWatcher _mcpeKeyWatcher;
    public final int allowedLength;
    public final boolean limitInput;

    public interface MCPEKeyWatcher {
        void onBackKeyPressed();

        void onDeleteKeyPressed();
    }

    public TextInputProxyEditTextbox(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        this._mcpeKeyWatcher = null;
        this.allowedLength = 160;
        this.limitInput = false;
    }

    public TextInputProxyEditTextbox(Context context, AttributeSet attrs) {
        super(context, attrs);
        this._mcpeKeyWatcher = null;
        this.allowedLength = 160;
        this.limitInput = false;
    }

    public TextInputProxyEditTextbox(Context context, int allowedLength, boolean limitInput) {
        super(context);
        InputFilter[] filters;
        this._mcpeKeyWatcher = null;
        this.allowedLength = allowedLength;
        this.limitInput = limitInput;
        if (limitInput) {
            filters = new InputFilter[]{new InputFilter.LengthFilter(this.allowedLength), new InputFilter() { // from class: com.mojang.minecraftpe.TextInputProxyEditTextbox.1
                @Override // android.text.InputFilter
                public CharSequence filter(CharSequence source, int start, int end, Spanned dest, int dstart, int dend) {
                    return (source.equals("") || source.toString().matches("^[a-zA-Z0-9_ -^~'.,;!#&()=`{}]*")) ? source : "";
                }
            }};
        } else {
            filters = new InputFilter[]{new InputFilter.LengthFilter(this.allowedLength)};
        }
        setFilters(filters);
    }

    @Override // android.widget.TextView, android.view.View
    public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
        return new MCPEInputConnection(super.onCreateInputConnection(outAttrs), true, this);
    }

    @Override // android.widget.TextView, android.view.View
    public boolean onKeyPreIme(int keyCode, KeyEvent event) {
        if (keyCode != 4 || event.getAction() != 1) {
            return super.dispatchKeyEvent(event);
        }
        if (this._mcpeKeyWatcher != null) {
            this._mcpeKeyWatcher.onBackKeyPressed();
        }
        return false;
    }

    public void setOnMCPEKeyWatcher(MCPEKeyWatcher mcpeKeyWatcher) {
        this._mcpeKeyWatcher = mcpeKeyWatcher;
    }

    private class MCPEInputConnection extends InputConnectionWrapper {
        TextInputProxyEditTextbox textbox;

        public MCPEInputConnection(InputConnection target, boolean mutable, TextInputProxyEditTextbox textbox) {
            super(target, mutable);
            this.textbox = textbox;
        }

        @Override // android.view.inputmethod.InputConnectionWrapper, android.view.inputmethod.InputConnection
        public boolean sendKeyEvent(KeyEvent event) {
            if (this.textbox.getText().length() != 0 || event.getAction() != 0 || event.getKeyCode() != 67) {
                return super.sendKeyEvent(event);
            }
            if (TextInputProxyEditTextbox.this._mcpeKeyWatcher != null) {
                TextInputProxyEditTextbox.this._mcpeKeyWatcher.onDeleteKeyPressed();
            }
            return false;
        }
    }
}
