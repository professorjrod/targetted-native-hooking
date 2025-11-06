#!/bin/bash

set -e

MODULE_PKG="com.xposed.clashroyalebypass"
TARGET_PKG="com.supercell.clashroyale"
APK_PATH="app/build/outputs/apk/debug/app-debug.apk"

echo "========================================="
echo "Clash Royale Bypass - Module Test Script"
echo "========================================="
echo ""

# Check if ADB is available
if ! command -v adb &> /dev/null; then
    echo "❌ ADB not found. Please install Android SDK Platform Tools."
    exit 1
fi

# Check device connection
echo "📱 Checking device connection..."
DEVICES=$(adb devices | grep -v "List" | grep "device$" | wc -l)
if [ "$DEVICES" -eq 0 ]; then
    echo "❌ No devices connected"
    echo ""
    echo "Please connect your device or start an emulator:"
    echo "  emulator -avd <your_avd_name> -writable-system"
    exit 1
fi

DEVICE=$(adb devices | grep "device$" | head -n1 | awk '{print $1}')
echo "✅ Connected to: $DEVICE"
echo ""

# Check if APK exists
echo "📦 Checking APK..."
if [ ! -f "$APK_PATH" ]; then
    echo "❌ APK not found at $APK_PATH"
    echo "Building module..."
    ./gradlew assembleDebug
fi
echo "✅ APK found: $APK_PATH"
echo ""

# Verify APK contents
echo "🔍 Verifying APK contents..."
NATIVE_LIB=$(unzip -l "$APK_PATH" | grep "libclash_bypass.so" | wc -l)
XPOSED_INIT=$(unzip -l "$APK_PATH" | grep "assets/xposed_init" | wc -l)
NATIVE_INIT=$(unzip -l "$APK_PATH" | grep "assets/native_init" | wc -l)

if [ "$NATIVE_LIB" -eq 0 ]; then
    echo "❌ Native library not found in APK"
    exit 1
fi
if [ "$XPOSED_INIT" -eq 0 ]; then
    echo "❌ xposed_init not found in APK"
    exit 1
fi
if [ "$NATIVE_INIT" -eq 0 ]; then
    echo "❌ native_init not found in APK"
    exit 1
fi

echo "✅ libclash_bypass.so found"
echo "✅ xposed_init found"
echo "✅ native_init found"
echo ""

# Check xposed_init content
XPOSED_CLASS=$(unzip -p "$APK_PATH" assets/xposed_init)
echo "📄 xposed_init points to: $XPOSED_CLASS"
NATIVE_LIB_NAME=$(unzip -p "$APK_PATH" assets/native_init)
echo "📄 native_init points to: $NATIVE_LIB_NAME"
echo ""

# Install module
echo "📲 Installing module..."
if adb install -r "$APK_PATH" 2>&1 | grep -q "Success"; then
    echo "✅ Module installed"
else
    echo "❌ Installation failed"
    exit 1
fi
echo ""

# Check if module is installed
echo "🔍 Verifying module installation..."
if adb shell pm list packages | grep -q "$MODULE_PKG"; then
    echo "✅ Module package found: $MODULE_PKG"
else
    echo "❌ Module package not found"
    exit 1
fi
echo ""

# Check LSPosed
echo "🔍 Checking LSPosed..."
if adb shell pm list packages | grep -q "org.lsposed.manager"; then
    echo "✅ LSPosed Manager found"
else
    echo "⚠️  LSPosed Manager not found"
    echo "Please install LSPosed Framework first:"
    echo "  https://github.com/LSPosed/LSPosed/releases"
    echo ""
fi

# Check if target app is installed
echo "🔍 Checking target app..."
if adb shell pm list packages | grep -q "$TARGET_PKG"; then
    echo "✅ Clash Royale installed"
else
    echo "⚠️  Clash Royale not installed"
    echo "Install Clash Royale to test the module"
    echo ""
fi

echo ""
echo "========================================="
echo "✅ Module is ready for testing!"
echo "========================================="
echo ""
echo "Next steps:"
echo "1. Open LSPosed Manager on your device"
echo "2. Go to Modules tab"
echo "3. Enable 'Clash Royale Bypass'"
echo "4. Set scope to 'Clash Royale'"
echo "5. Reboot device: adb reboot"
echo ""
echo "After reboot, monitor logs:"
echo "  adb logcat -s ClashBypass:*"
echo ""
echo "Then launch Clash Royale:"
echo "  adb shell am start -n $TARGET_PKG/.GameApp"
echo ""
