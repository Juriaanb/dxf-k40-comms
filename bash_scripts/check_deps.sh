#!/bin/bash

# Check Dependencies
# Verifies all required dependencies are installed

echo "=== Dependency Check ==="

# Function to check if command exists
check_command() {
    if command -v "$1" &> /dev/null; then
        echo "  ✓ $1"
        return 0
    else
        echo "  ✗ $1"
        return 1
    fi
}

# Function to check if package is installed (Fedora)
check_package() {
    if rpm -q "$1" &> /dev/null; then
        echo "  ✓ $1"
        return 0
    else
        echo "  ✗ $1"
        return 1
    fi
}

MISSING=0

echo "Build Tools:"
check_command cmake || MISSING=$((MISSING+1))
check_command make || MISSING=$((MISSING+1))
check_command gcc || MISSING=$((MISSING+1))
check_command g++ || MISSING=$((MISSING+1))

echo ""
echo "Development Libraries:"
check_package mesa-libGL-devel || MISSING=$((MISSING+1))
check_package mesa-libGLU-devel || MISSING=$((MISSING+1))
check_package glfw-devel || MISSING=$((MISSING+1))
check_package opencascade-devel || MISSING=$((MISSING+1))

echo ""
echo "Utilities:"
check_command git || MISSING=$((MISSING+1))
check_command pkg-config || MISSING=$((MISSING+1))

echo ""
echo "Project Files:"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

if [ -f "$PROJECT_DIR/CMakeLists.txt" ]; then
    echo "  ✓ CMakeLists.txt"
else
    echo "  ✗ CMakeLists.txt"
    MISSING=$((MISSING+1))
fi

if [ -d "$PROJECT_DIR/external/glad" ]; then
    echo "  ✓ GLAD"
else
    echo "  ✗ GLAD (run glad_gen.sh)"
    MISSING=$((MISSING+1))
fi

echo ""
if [ $MISSING -eq 0 ]; then
    echo "All dependencies satisfied!"
else
    echo "Missing $MISSING dependencies"
    echo "Run 'Setup Project Dependencies' to install missing packages"
fi