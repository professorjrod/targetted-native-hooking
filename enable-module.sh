#!/bin/bash

# Script to enable Clash Royale Bypass module in LSPosed

echo "🔧 Configuring LSPosed..."

# Create modules config if it doesn't exist
adb shell su -c "mkdir -p /data/adb/lspd/config"

# Enable module for Clash Royale
adb shell su -c "cat > /data/adb/lspd/config/modules.json << 'EOF'
{
  \"com.xposed.clashroyalebypass\": {
    \"enabled\": true,
    \"scope\": [\"com.supercell.clashroyale\"]
  }
}
EOF"

echo "✅ Module configuration created"
echo ""
echo "Manual steps required:"
echo "1. LSPosed Manager is now open on your device"
echo "2. Tap 'Modules' tab"
echo "3. Enable 'Clash Royale Bypass'"
echo "4. Tap the module → Enable for 'Clash Royale' app"
echo "5. Force stop Clash Royale: adb shell am force-stop com.supercell.clashroyale"
echo ""
echo "Then test with:"
echo "  adb logcat -c && adb logcat -s ClashBypass:* &"
echo "  adb shell monkey -p com.supercell.clashroyale 1"
