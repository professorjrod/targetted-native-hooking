# Keep Xposed API references but don't include the classes
-keep class com.xposed.clashroyalebypass.** { *; }
-keep interface de.robv.android.xposed.** { *; }

# Don't include Xposed API implementation
-dontwarn de.robv.android.xposed.**
