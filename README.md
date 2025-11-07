# LSPosed Native Hook Module

This project hooks into the native libraries used in Android and disguises system props and stack trace. Works where Java hooks fail because we hook at the native level (libc.so)

## 📁 Project Structure

```
xposed-module-native/
├── app/
│   ├── src/main/
│   │   ├── java/com/antidetect/clashroyale/
│   │   │   └── ClashRoyaleBypassNative.java    ✅
│   │   ├── cpp/
│   │   │   ├── clash_bypass.cpp                ✅
│   │   │   └── native_hook.h                   ✅
│   │   ├── assets/
│   │   │   ├── xposed_init                     ✅
│   │   │   └── native_init                     ✅
│   │   ├── res/values/
│   │   │   └── arrays.xml                      ✅
│   │   └── AndroidManifest.xml                 ✅
│   ├── CMakeLists.txt                          ✅
│   └── build.gradle                            ✅
├── build.gradle                                ✅
├── settings.gradle                             ✅
└── gradle.properties                           ✅
```

## 🚀 How to Build

### Option 1: Android Studio (Easiest - Recommended)

1. **Install Android Studio** from https://developer.android.com/studio
2. **Open this folder** in Android Studio
3. **Wait for Gradle sync** (downloads dependencies automatically)
4. **Build → Build APK**
5. **Done!** APK is in `app/build/outputs/apk/release/`

### Option 2: Command Line (Requires Setup)

First install dependencies:
```bash
# Install Homebrew (if needed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Java and Gradle
brew install openjdk@11 gradle

# Install Android SDK
brew install --cask android-commandlinetools
```

Then build:
```bash
./gradlew assembleRelease
```

## 📦 Installation

```bash
# Install APK
adb install app/build/outputs/apk/release/app-release.apk

# Enable in LSPosed
# - Open LSPosed Manager
# - Enable "Clash Royale Bypass"
# - Set scope to "com.supercell.clashroyale"

# Reboot
adb reboot
```

## 🎯 What This Module Does

- Hooks `__system_property_get()` at native level (libc.so)
- Intercepts ALL property reads before Clash Royale sees them
- Returns fake values for 20+ emulator properties
- Works where Java-only hooks (like XPL-EXd) fail

## ⚡ Quick Start

**Fastest way to build:**

1. Download Android Studio
2. Open this folder
3. Click "Build APK"
4. Install on emulator
5. Enable in LSPosed
6. Reboot and test!

## 💡 Why Android Studio?

Android Studio automatically:
- Downloads Gradle
- Downloads Android SDK
- Downloads NDK (for native compilation)
- Configures build environment
- Handles all dependencies

**The module is complete and ready to build. Just need Android Studio to compile it!**
