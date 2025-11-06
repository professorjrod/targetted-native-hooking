# Clash Royale Bypass - Native Hook Module Build Guide

## Overview

This is a complete LSPosed Xposed module with **native hooks** that intercepts `__system_property_get()` at the libc.so level to fake emulator properties for Clash Royale.

## Project Structure

```
xposed-module-native/
├── native_hook.h              # LSPosed native hook header
├── clash_bypass.cpp           # Native C++ hook implementation
├── ClashRoyaleBypassNative.java  # Java entry point
├── CMakeLists.txt             # CMake build configuration
├── build.gradle               # Android Gradle build file
├── AndroidManifest.xml        # Android manifest with Xposed metadata
├── assets/
│   ├── xposed_init            # Xposed module entry class
│   └── native_init            # Native library name for LSPosed
└── res/
    └── values/
        └── arrays.xml         # Xposed scope configuration
```

## Prerequisites

1. **Android Studio** (latest version)
2. **Android NDK** (for native compilation)
3. **Java JDK 8 or higher**
4. **Xposed API** (automatically downloaded by Gradle)

## Build Steps

### Option 1: Using Android Studio (Recommended)

1. **Open Android Studio**

2. **Import the project:**
   - File → Open
   - Navigate to `/Users/jared/CascadeProjects/clash-royale-poc/kernel-build/xposed-module-native`
   - Click "Open"

3. **Configure NDK:**
   - File → Project Structure → SDK Location
   - Set Android NDK location (or let Android Studio download it)

4. **Sync Gradle:**
   - Click "Sync Project with Gradle Files" button
   - Wait for dependencies to download

5. **Build the APK:**
   - Build → Build Bundle(s) / APK(s) → Build APK(s)
   - Or run: `./gradlew assembleRelease`

6. **Find the APK:**
   - Located in: `app/build/outputs/apk/release/app-release.apk`

### Option 2: Command Line Build

```bash
cd /Users/jared/CascadeProjects/clash-royale-poc/kernel-build/xposed-module-native

# Install dependencies (first time only)
./gradlew build

# Build release APK
./gradlew assembleRelease

# Output: app/build/outputs/apk/release/app-release.apk
```

## Installation

1. **Push APK to device:**
   ```bash
   adb install app/build/outputs/apk/release/app-release.apk
   ```

2. **Enable in LSPosed:**
   - Open LSPosed Manager
   - Go to "Modules"
   - Enable "Clash Royale Bypass"
   - Set scope to "com.supercell.clashroyale"

3. **Reboot:**
   ```bash
   adb reboot
   ```

## How It Works

### Native Hook Flow:

1. **LSPosed loads the module** when Clash Royale starts
2. **Java code** (`ClashRoyaleBypassNative.java`) calls `System.loadLibrary("clash_bypass")`
3. **LSPosed calls** `native_init()` in `clash_bypass.cpp`
4. **native_init()** hooks `__system_property_get()` from libc.so
5. **Every property read** goes through our `fake_system_property_get()` function
6. **We return fake values** for emulator properties

### Properties Being Hooked:

```cpp
ro.boot.qemu = "0"                    // Critical
ro.boot.hardware = "exynos2100"       // Critical
ro.boot.serialno = "R5CR8348495"      // Critical
ro.serialno = "R5CR8348495"
ro.hardware = "exynos2100"
ro.product.board = "universal2100"
ro.build.characteristics = "default"
qemu.hw.mainkeys = ""
qemu.sf.lcd_density = ""
// ... and more
```

## Verification

After installation and reboot:

```bash
# Check if module is loaded
adb logcat | grep "ClashBypass"

# You should see:
# ClashBypassNative: Clash Royale detected, loading native library...
# ClashBypass: ClashBypass native_init called
# ClashBypass: Found __system_property_get at 0x...
# ClashBypass: Successfully hooked __system_property_get

# Launch Clash Royale
adb shell am start -n com.supercell.clashroyale/com.supercell.titan.GameApp

# Check for hook logs
adb logcat | grep "Hooked property"

# You should see:
# ClashBypass: Hooked property: ro.boot.qemu = 0
# ClashBypass: Hooked property: ro.boot.hardware = exynos2100
# ClashBypass: Hooked property: ro.boot.serialno = R5CR8348495
```

## Troubleshooting

### Build Errors:

**NDK not found:**
```
File → Project Structure → SDK Location → Android NDK location
```

**Gradle sync failed:**
```bash
./gradlew clean
./gradlew build
```

**CMake errors:**
- Ensure CMake 3.10.2+ is installed
- Check NDK version compatibility

### Runtime Errors:

**Module not loading:**
- Check LSPosed logs: LSPosed Manager → Logs
- Verify module is enabled for Clash Royale
- Ensure scope is set correctly

**Native library not found:**
- Check if `libclash_bypass.so` is in APK: `unzip -l app-release.apk | grep libclash_bypass`
- Verify `assets/native_init` contains `libclash_bypass.so`
- Check `extractNativeLibs="false"` in AndroidManifest.xml

**Hook not working:**
- Check logcat for "Successfully hooked __system_property_get"
- If not found, LSPosed may not be calling native_init
- Verify LSPosed version >= 1.8.6

### Clash Royale Still Crashing:

If Clash Royale still crashes with `y.F: 02`:

1. **Check hook logs** - Are properties being hooked?
2. **Add more properties** - Clash Royale might check additional properties
3. **Check timing** - Hook might be loading too late

## Advanced: Adding More Properties

Edit `clash_bypass.cpp` and add to the `fake_props` array:

```cpp
static const PropertyMapping fake_props[] = {
    // ... existing properties ...
    {"your.new.property", "fake_value"},
};
```

Rebuild and reinstall.

## Project Files Summary

### Native Code (C++):
- **native_hook.h**: LSPosed API definitions
- **clash_bypass.cpp**: Hook implementation

### Java Code:
- **ClashRoyaleBypassNative.java**: Module entry point

### Build Configuration:
- **CMakeLists.txt**: Native library build
- **build.gradle**: Android app build
- **AndroidManifest.xml**: App metadata

### Assets:
- **xposed_init**: Java class name
- **native_init**: Native library name

## Expected Result

With this module:
- ✅ Clash Royale launches without crash
- ✅ All emulator properties return fake values
- ✅ Bypass test score: 18-20/20
- ✅ No `y.F: 02` error

## Next Steps

1. Build the module in Android Studio
2. Install on emulator
3. Enable in LSPosed
4. Reboot
5. Test Clash Royale

Good luck! 🎮
