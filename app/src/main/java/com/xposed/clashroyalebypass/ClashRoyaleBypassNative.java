package com.xposed.clashroyalebypass;

import android.util.Log;
import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class ClashRoyaleBypassNative implements IXposedHookLoadPackage {
    private static final String TAG = "ClashBypass";
    private static final String TARGET_PACKAGE = "com.supercell.clashroyale";
    
    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam lpparam) throws Throwable {
        // Only load for target package
        if (!lpparam.packageName.equals(TARGET_PACKAGE)) {
            return;
        }
        
        Log.i(TAG, "Loading native library for " + TARGET_PACKAGE);
        
        try {
            // Load native library - this triggers native_init
            System.loadLibrary("clash_bypass");
            Log.i(TAG, "Native library loaded successfully");
        } catch (Throwable e) {
            Log.e(TAG, "Failed to load native library", e);
        }
    }
}
