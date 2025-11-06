package com.xposed.clashroyalebypass

import android.util.Log
import de.robv.android.xposed.IXposedHookLoadPackage
import de.robv.android.xposed.callbacks.XC_LoadPackage

class MainHook : IXposedHookLoadPackage {
    companion object {
        private const val TAG = "ClashBypass"
        private const val TARGET_PACKAGE = "com.supercell.clashroyale"
    }

    override fun handleLoadPackage(lpparam: XC_LoadPackage.LoadPackageParam) {
        if (lpparam.packageName != TARGET_PACKAGE) {
            return
        }

        Log.i(TAG, "Loading native library for $TARGET_PACKAGE")

        try {
            System.loadLibrary("clash_bypass")
            Log.i(TAG, "Native library loaded successfully")
        } catch (e: Throwable) {
            Log.e(TAG, "Failed to load native library", e)
        }
    }
}
