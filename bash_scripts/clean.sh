#!/bin/bash

# Clean Build
# Removes build artifacts and temporary files

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

echo "=== Cleaning Build ==="

# Remove build directory
if [ -d "$BUILD_DIR" ]; then
    echo "Removing build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
    echo "Build directory removed"
else
    echo "Build directory does not exist"
fi

# Remove any CMake cache files in project root
if [ -f "$PROJECT_DIR/CMakeCache.txt" ]; then
    echo "Removing CMakeCache.txt"
    rm -f "$PROJECT_DIR/CMakeCache.txt"
fi

if [ -d "$PROJECT_DIR/CMakeFiles" ]; then
    echo "Removing CMakeFiles directory"
    rm -rf "$PROJECT_DIR/CMakeFiles"
fi

# Remove any backup files
find "$PROJECT_DIR" -name "*~" -type f -delete 2>/dev/null
find "$PROJECT_DIR" -name "*.bak" -type f -delete 2>/dev/null

echo "Clean complete"