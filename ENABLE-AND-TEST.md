# Quick Enable & Test Guide

## ✅ Status: LSPosed Installed!

LSPosed v1.9.2 is now running on your emulator.
LSPosed Manager is currently open on your device.

## 📱 Enable Module (2 minutes)

### In LSPosed Manager (on device):

1. **Tap "Modules" tab** at bottom
2. **Find "Clash Royale Bypass"** in list
3. **Toggle it ON** (enable the module)
4. **Tap the module name** to open details
5. **Tap "Enable for app"** or "Application list"
6. **Check "Clash Royale"** in the app list
7. **Go back** to modules list

### Verify Configuration:
```bash
# Check if module is in LSPosed database
adb shell su -c "ls -la /data/adb/lspd/config/"
```

## 🧪 Test the Module

### Step 1: Clear logs and start monitoring
```bash
adb logcat -c
adb logcat -s ClashBypass:* Xposed:* LSPosed:* | grep -E "(ClashBypass|module|hook)"
```

### Step 2: Launch Clash Royale (in another terminal)
```bash
# Use the correct activity name
adb shell monkey -p com.supercell.clashroyale 1

# Or if that doesn't work
adb shell am start -n com.supercell.clashroyale/com.supercell.titan.GameApp
```

## ✅ Expected Logs

You should see logs like:
```
LSPosed: Loading module com.xposed.clashroyalebypass
ClashBypass: Loading native library for com.supercell.clashroyale
ClashBypass: Native library loaded successfully
ClashBypass: ClashBypass native_init called
ClashBypass: Found __system_property_get at 0x...
ClashBypass: Successfully hooked __system_property_get
ClashBypass: Hooked property: ro.boot.qemu = 0
ClashBypass: Hooked property: ro.hardware = exynos2100
```

## 🐛 If No Logs Appear

### Check Module is Enabled:
```bash
# Force stop app first
adb shell am force-stop com.supercell.clashroyale

# Check LSPosed status
adb logcat -s LSPosed:* | head -20

# Relaunch
adb shell monkey -p com.supercell.clashroyale 1
```

### Check SELinux:
```bash
# If nothing works, try permissive mode
adb shell su -c setenforce 0

# Then relaunch
adb shell am force-stop com.supercell.clashroyale
adb shell monkey -p com.supercell.clashroyale 1
```

### Verify Module Loaded:
```bash
# Check all Xposed-related logs
adb logcat -s Xposed:* LSPosed:* ClashBypass:* | head -50
```

## 🔄 If Still Not Working

1. **Reboot device** (sometimes needed for first-time LSPosed setup):
   ```bash
   adb reboot
   adb wait-for-device
   sleep 20
   ```

2. **Re-enable module** in LSPosed Manager

3. **Check module loading**:
   ```bash
   adb logcat | grep -i "clashroyale.*xposed\|xposed.*clashroyale"
   ```

## 📊 Success Indicators

✅ **Module loads**: See "Loading module com.xposed.clashroyalebypass" in logs
✅ **Native lib loads**: See "Native library loaded successfully"
✅ **Hook installs**: See "Successfully hooked __system_property_get"
✅ **Properties spoofed**: See "Hooked property: ro.boot.qemu = 0"
✅ **Clash Royale runs**: App doesn't crash with y.F: 02 error

## 🎯 Complete Test Flow

```bash
# Terminal 1: Monitor logs
adb logcat -c
adb logcat -s ClashBypass:* LSPosed:* Xposed:*

# Terminal 2: Launch app
adb shell am force-stop com.supercell.clashroyale
sleep 2
adb shell monkey -p com.supercell.clashroyale 1

# Watch Terminal 1 for hook messages
```
