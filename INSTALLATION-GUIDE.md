# Installation & Testing Guide

## ✅ Module Status: BUILDS SUCCESSFULLY

Your APK is ready at: `app/build/outputs/apk/debug/app-debug.apk`

## 📋 Prerequisites

1. **Rooted Android device/emulator** with Magisk installed
2. **LSPosed Framework** installed (Zygisk or Riru variant)
3. **ADB connection** to device

## 🔌 Setup Device Connection

### Connect Your Emulator
```bash
# List running emulators
emulator -list-avds

# Start your emulator with writable system
emulator -avd <avd_name> -writable-system
```

### Verify Connection
```bash
adb devices
# Should show: emulator-5554    device
```

## 📦 Install Module

```bash
cd /Users/jared/CascadeProjects/clash-royale-poc/kernel-build/xposed-module-native

# Install APK
adb install -r app/build/outputs/apk/debug/app-debug.apk

# Verify installation
adb shell pm list packages | grep clashroyalebypass
# Should show: package:com.xposed.clashroyalebypass
```

## ⚙️ Enable in LSPosed

### Via ADB (Command Line)
```bash
# Open LSPosed Manager
adb shell am start -n org.lsposed.manager/.ui.activity.MainActivity

# Then manually:
# 1. Enable "Clash Royale Bypass" module
# 2. Set scope to "Clash Royale" (com.supercell.clashroyale)
# 3. Reboot device
```

### Via GUI
1. Open **LSPosed Manager** app on device
2. Go to **Modules** tab
3. Enable **Clash Royale Bypass**
4. Tap the module → **Enable for app** → Select **Clash Royale**
5. Reboot device

## 📊 Verify Module Loading

### Check Logs
```bash
# Watch module logs in real-time
adb logcat -s ClashBypass:* | grep -E "(native_init|hooked|library)"

# Expected output:
# ClashBypass: ClashBypass native_init called
# ClashBypass: Found __system_property_get at 0x...
# ClashBypass: Successfully hooked __system_property_get
```

### Test Property Hooking
```bash
# Launch Clash Royale
adb shell am start -n com.supercell.clashroyale/.GameApp

# Monitor hooked properties
adb logcat -s ClashBypass:* | grep "Hooked property"

# Expected:
# ClashBypass: Hooked property: ro.boot.qemu = 0
# ClashBypass: Hooked property: ro.hardware = exynos2100
```

## 🐛 Troubleshooting

### Module Not Loading

**Check if module is installed:**
```bash
adb shell pm list packages | grep clashroyalebypass
```

**Check if LSPosed sees it:**
```bash
adb shell cat /data/adb/lspd/config/modules.json
# Should contain: "com.xposed.clashroyalebypass"
```

### Native Library Not Found

**Verify .so file in APK:**
```bash
unzip -l app/build/outputs/apk/debug/app-debug.apk | grep libclash_bypass
# Should show: lib/arm64-v8a/libclash_bypass.so
```

**Check native_init export:**
```bash
nm -D app/build/intermediates/cxx/Debug/*/obj/arm64-v8a/libclash_bypass.so | grep native_init
# Should show: 0000000000001330 T native_init
```

### Clash Royale Still Crashes

**Check crash reason:**
```bash
adb logcat -b crash | grep -A 20 "clashroyale"
```

**Common issues:**
1. **Module not enabled for Clash Royale** - Check LSPosed scope
2. **SELinux blocking** - Set to permissive: `adb shell setenforce 0`
3. **Wrong ABI** - Module only built for arm64-v8a

## 🔄 Rebuild & Reinstall

```bash
# Clean build
./gradlew clean

# Rebuild
./gradlew assembleDebug

# Uninstall old version
adb uninstall com.xposed.clashroyalebypass

# Install new version
adb install app/build/outputs/apk/debug/app-debug.apk

# Reboot
adb reboot
```

## 📝 Full Test Workflow

```bash
# 1. Build module
./gradlew assembleDebug

# 2. Install
adb install -r app/build/outputs/apk/debug/app-debug.apk

# 3. Enable in LSPosed (manual step - open LSPosed Manager)

# 4. Reboot
adb reboot

# 5. Wait for reboot, then monitor logs
adb wait-for-device
adb logcat -c  # Clear logs
adb logcat -s ClashBypass:*

# 6. Launch Clash Royale
adb shell am start -n com.supercell.clashroyale/.GameApp

# 7. Watch for hook messages
```

## ✅ Success Indicators

You'll know the module works when you see:
```
ClashBypass: Loading native library for com.supercell.clashroyale
ClashBypass: Native library loaded successfully
ClashBypass: ClashBypass native_init called
ClashBypass: Found __system_property_get at 0x...
ClashBypass: Successfully hooked __system_property_get
ClashBypass: Hooked property: ro.boot.qemu = 0
ClashBypass: Hooked property: ro.hardware = exynos2100
```

And Clash Royale launches without the `y.F: 02` error!
