package com.antidetect.clashroyale;

import android.util.Log;
import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class ClashRoyaleBypassNative implements IXposedHookLoadPackage {
    private static final String TAG = "ClashBypassNative";
    private static final String TARGET_PACKAGE = "com.supercell.clashroyale";
    
    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam lpparam) throws Throwable {
        // Only hook Clash Royale
        if (!lpparam.packageName.equals(TARGET_PACKAGE)) {
            return;
        }
        
        Log.i(TAG, "Clash Royale detected, loading native library...");
        
        try {
            // Load our native library which contains the native_init function
            System.loadLibrary("clash_bypass");
            Log.i(TAG, "Native library loaded successfully");
        } catch (Throwable e) {
            Log.e(TAG, "Failed to load native library", e);
        }
    }
}
