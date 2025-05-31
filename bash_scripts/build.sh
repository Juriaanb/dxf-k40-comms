#!/bin/bash

# Build Project
# Compiles the STEP Viewer application

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

echo "=== Building STEP Viewer ==="
echo "Project: $PROJECT_DIR"
echo "Build: $BUILD_DIR"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo "Configuring with CMake..."
if ! cmake -DCMAKE_BUILD_TYPE=Debug ..; then
    echo "ERROR: CMake configuration failed"
    exit 1
fi

# Build
echo "Building..."
if ! make -j$(nproc); then
    echo "ERROR: Build failed"
    exit 1
fi

echo "Build successful!"

# Check if binary exists
if [ -f "./StepViewer" ]; then
    echo "Binary: $BUILD_DIR/StepViewer"
    echo "Size: $(du -h ./StepViewer | cut -f1)"
else
    echo "WARNING: Binary not found"
    exit 1
fi