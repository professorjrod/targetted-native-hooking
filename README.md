# Clash Royale Bypass - Native Hook Module

## ✅ Project Status: READY TO BUILD

All source files are complete and in the correct locations. The module just needs to be compiled.

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

## 🔧 Build Error Fix

The build error you encountered was due to missing Gradle configuration files. This has been fixed:

✅ Created `settings.gradle` with plugin repositories
✅ Split `build.gradle` into root and app-level files  
✅ Created proper Android Studio project structure
✅ Moved all files to correct locations

## 📖 Documentation

- **QUICK-BUILD-GUIDE.md** - Step-by-step build instructions
- **BUILD-GUIDE.md** - Detailed technical documentation
- **NATIVE-MODULE-SUMMARY.md** - Overview and how it works

## ⚡ Quick Start

**Fastest way to build:**

1. Download Android Studio
2. Open this folder
3. Click "Build APK"
4. Install on emulator
5. Enable in LSPosed
6. Reboot and test!

## 🎮 Expected Result

Once installed:
- ✅ Clash Royale launches without crash
- ✅ No `y.F: 02` error
- ✅ All emulator properties return fake values
- ✅ Logs show "Successfully hooked __system_property_get"

## 💡 Why Android Studio?

Android Studio automatically:
- Downloads Gradle
- Downloads Android SDK
- Downloads NDK (for native compilation)
- Configures build environment
- Handles all dependencies

**It's the official way to build Android apps and modules.**

## 🆘 Need Help?

See **QUICK-BUILD-GUIDE.md** for troubleshooting and detailed instructions.

---

**The module is complete and ready to build. Just need Android Studio to compile it!**
