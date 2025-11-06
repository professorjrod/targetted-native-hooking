package com.xposed.clashroyalebypass;

import android.util.Log;
import de.robv.android.xposed.IXposedHookZygoteInit;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class XposedInit implements IXposedHookZygoteInit {
    private static final String TAG = "ClashBypass";
    
    @Override
    public void initZygote(StartupParam startupParam) throws Throwable {
        Log.i(TAG, "Zygote init called - NATIVE ONLY MODE");
        
        // Load native library early, before app starts
        // NO JAVA HOOKS - stay out of stack traces!
        try {
            System.loadLibrary("clash_bypass");
            Log.i(TAG, "Native library loaded in zygote");
        } catch (Throwable e) {
            Log.e(TAG, "Failed to load native library in zygote: " + e.getMessage(), e);
        }
    }
}
