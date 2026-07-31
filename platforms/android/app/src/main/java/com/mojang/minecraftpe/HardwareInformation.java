package com.mojang.minecraftpe;

import android.os.Build;
import java.io.File;
import java.io.FileFilter;
import java.util.regex.Pattern;

/* loaded from: classes.dex */
public class HardwareInformation {
    public static String getDeviceModelName() {
        String manufacturer = Build.MANUFACTURER;
        String model = Build.MODEL;
        if (model.startsWith(manufacturer)) {
            String returnString = model.toUpperCase();
            return returnString;
        }
        String returnString2 = manufacturer.toUpperCase() + " " + model;
        return returnString2;
    }

    public static String getAndroidVersion() {
        return Build.VERSION.RELEASE;
    }

    public static String getCPU() {
        return Build.CPU_ABI;
    }

    public static int getNumCores() {
        try {
            File dir = new File("/sys/devices/system/cpu/");
            File[] files = dir.listFiles(new FileFilter() { // from class: com.mojang.minecraftpe.HardwareInformation.1CpuFilter
                @Override // java.io.FileFilter
                public boolean accept(File pathname) {
                    return Pattern.matches("cpu[0-9]+", pathname.getName());
                }
            });
            return files.length;
        } catch (Exception e) {
            return 1;
        }
    }
}
