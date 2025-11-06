#!/bin/bash

echo "========================================="
echo "Simple Build Script (No Android Studio)"
echo "========================================="
echo ""

echo "This module requires:"
echo "1. Android Studio with NDK"
echo "2. Gradle with Android Gradle Plugin"
echo ""
echo "Since building from scratch is complex, here are your options:"
echo ""
echo "Option 1: Use Android Studio (Recommended)"
echo "  - Install Android Studio from https://developer.android.com/studio"
echo "  - Open this project folder"
echo "  - Let it download dependencies"
echo "  - Build → Build APK"
echo ""
echo "Option 2: Use Pre-built Template"
echo "  - Download a working Xposed module template"
echo "  - Replace the source files with ours"
echo "  - Build with existing gradle setup"
echo ""
echo "Option 3: Manual APK Creation (Advanced)"
echo "  - Compile native library separately with NDK"
echo "  - Create APK structure manually"
echo "  - Sign with apksigner"
echo ""
echo "Would you like me to create a Docker-based build environment?"
echo "This would allow building without installing Android Studio."
echo ""
read -p "Create Docker build environment? (y/N) " -n 1 -r
echo ""

if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "Creating Dockerfile..."
    
    cat > Dockerfile << 'DOCKERFILE_EOF'
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    openjdk-11-jdk \
    wget \
    unzip \
    git \
    && rm -rf /var/lib/apt/lists/*

# Install Android SDK
ENV ANDROID_SDK_ROOT=/opt/android-sdk
RUN mkdir -p ${ANDROID_SDK_ROOT}/cmdline-tools && \
    cd ${ANDROID_SDK_ROOT}/cmdline-tools && \
    wget https://dl.google.com/android/repository/commandlinetools-linux-9477386_latest.zip && \
    unzip commandlinetools-linux-9477386_latest.zip && \
    rm commandlinetools-linux-9477386_latest.zip && \
    mv cmdline-tools latest

# Install Android SDK components
RUN yes | ${ANDROID_SDK_ROOT}/cmdline-tools/latest/bin/sdkmanager --licenses && \
    ${ANDROID_SDK_ROOT}/cmdline-tools/latest/bin/sdkmanager \
    "platform-tools" \
    "platforms;android-34" \
    "build-tools;34.0.0" \
    "ndk;25.2.9519653" \
    "cmake;3.22.1"

# Set environment
ENV PATH=${PATH}:${ANDROID_SDK_ROOT}/platform-tools:${ANDROID_SDK_ROOT}/cmdline-tools/latest/bin

WORKDIR /project

CMD ["/bin/bash"]
DOCKERFILE_EOF

    echo ""
    echo "✅ Dockerfile created!"
    echo ""
    echo "To build with Docker:"
    echo "  1. docker build -t android-builder ."
    echo "  2. docker run -v \$(pwd):/project android-builder ./gradlew assembleRelease"
    echo ""
else
    echo ""
    echo "Please install Android Studio to build this module."
    echo "See BUILD-GUIDE.md for detailed instructions."
    echo ""
fi
