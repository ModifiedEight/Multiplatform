package com.mojang.minecraftpe;

import android.content.Intent;
import android.net.Uri;

/* loaded from: classes.dex */
public class Minecraft_Market extends MainActivity {
    @Override // com.mojang.minecraftpe.MainActivity
    public void buyGame() {
        Uri buyLink = Uri.parse("market://details?id=com.mojang.minecraftpe");
        Intent marketIntent = new Intent("android.intent.action.VIEW", buyLink);
        startActivity(marketIntent);
    }
}
