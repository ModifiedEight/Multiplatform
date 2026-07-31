package com.mojang.android;

import android.widget.TextView;

/* loaded from: classes.dex */
public class TextViewReader implements StringValue {
    private TextView _view;

    public TextViewReader(TextView view) {
        this._view = view;
    }

    @Override // com.mojang.android.StringValue
    public String getStringValue() {
        return this._view.getText().toString();
    }
}
