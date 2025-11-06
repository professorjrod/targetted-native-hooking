#!/bin/bash

echo "========================================="
echo "Clash Royale Bypass - Project Setup"
echo "========================================="
echo ""

PROJECT_DIR="/Users/jared/CascadeProjects/clash-royale-poc/kernel-build/xposed-module-native"

echo "Creating Android Studio project structure..."
echo ""

# Create directory structure
mkdir -p "$PROJECT_DIR/app/src/main/java/com/antidetect/clashroyale"
mkdir -p "$PROJECT_DIR/app/src/main/cpp"
mkdir -p "$PROJECT_DIR/app/src/main/assets"
mkdir -p "$PROJECT_DIR/app/src/main/res/values"

# Move files to correct locations
echo "Moving files to Android Studio structure..."

# Java source
cp "$PROJECT_DIR/ClashRoyaleBypassNative.java" "$PROJECT_DIR/app/src/main/java/com/antidetect/clashroyale/"

# Native source
cp "$PROJECT_DIR/clash_bypass.cpp" "$PROJECT_DIR/app/src/main/cpp/"
cp "$PROJECT_DIR/native_hook.h" "$PROJECT_DIR/app/src/main/cpp/"
cp "$PROJECT_DIR/CMakeLists.txt" "$PROJECT_DIR/app/"

# Assets
cp "$PROJECT_DIR/assets/xposed_init" "$PROJECT_DIR/app/src/main/assets/"
cp "$PROJECT_DIR/assets/native_init" "$PROJECT_DIR/app/src/main/assets/"

# Resources
cp "$PROJECT_DIR/res/values/arrays.xml" "$PROJECT_DIR/app/src/main/res/values/"

# Build files
cp "$PROJECT_DIR/build.gradle" "$PROJECT_DIR/app/"
cp "$PROJECT_DIR/AndroidManifest.xml" "$PROJECT_DIR/app/src/main/"

# Create root build.gradle
cat > "$PROJECT_DIR/build.gradle" << 'EOF'
buildscript {
    repositories {
        google()
        mavenCentral()
    }
    dependencies {
        classpath 'com.android.tools.build:gradle:8.1.0'
    }
}

allprojects {
    repositories {
        google()
        mavenCentral()
    }
}

task clean(type: Delete) {
    delete rootProject.buildDir
}
EOF

# Create settings.gradle
cat > "$PROJECT_DIR/settings.gradle" << 'EOF'
rootProject.name = "ClashRoyaleBypass"
include ':app'
EOF

# Create gradle.properties
cat > "$PROJECT_DIR/gradle.properties" << 'EOF'
android.useAndroidX=true
android.enableJetifier=true
org.gradle.jvmargs=-Xmx2048m
EOF

echo ""
echo "✅ Project structure created!"
echo ""
echo "========================================="
echo "Next Steps"
echo "========================================="
echo ""
echo "1. Open Android Studio"
echo "2. File → Open → Select: $PROJECT_DIR"
echo "3. Wait for Gradle sync"
echo "4. Build → Build Bundle(s) / APK(s) → Build APK(s)"
echo ""
echo "Or build from command line:"
echo "  cd $PROJECT_DIR"
echo "  ./gradlew assembleRelease"
echo ""
echo "APK will be in: app/build/outputs/apk/release/app-release.apk"
echo ""
echo "See BUILD-GUIDE.md for detailed instructions"
echo ""
