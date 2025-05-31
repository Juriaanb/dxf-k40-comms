#!/bin/bash

# Setup Project Dependencies
# Installs all required packages and sets up GLAD

echo "=== Setting up STEP Viewer dependencies ==="

# Check if running on Fedora
if ! command -v dnf &> /dev/null; then
    echo "ERROR: This script is designed for Fedora. Please install dependencies manually."
    exit 1
fi

echo "Installing system dependencies..."
echo "You may be prompted for your sudo password..."

# Install development tools and dependencies
if ! sudo dnf install -y \
    cmake \
    gcc-c++ \
    make \
    git \
    pkg-config \
    meson \
    ninja-build \
    libffi-devel \
    expat-devel \
    libxml2-devel \
    graphviz \
    tree \
    htop \
    vim; then
    echo "ERROR: Failed to install system dependencies"
    exit 1
fi

echo "System dependencies installed"

# Setup local Wayland dependencies
echo "Setting up local Wayland libraries..."
mkdir -p external/wayland-local

cd external/wayland-local

# Download and build Wayland
if [ ! -d "wayland" ]; then
    echo "Downloading Wayland..."
    if ! git clone https://gitlab.freedesktop.org/wayland/wayland.git --depth 1; then
        echo "ERROR: Failed to download Wayland"
        exit 1
    fi
fi

if [ ! -d "wayland-install" ]; then
    echo "Building Wayland..."
    cd wayland
    
    # Clean any previous failed build
    rm -rf build
    
    meson setup build --prefix="$(pwd)/../wayland-install" -Ddocumentation=false
    if [ $? -eq 0 ]; then
        meson compile -C build
        if [ $? -eq 0 ]; then
            meson install -C build
            echo "Wayland built successfully"
        else
            echo "ERROR: Wayland compile failed"
            cd ..
            exit 1
        fi
    else
        echo "ERROR: Wayland setup failed"
        cd ..
        exit 1
    fi
    cd ..
fi

# Download wayland-protocols  
if [ ! -d "wayland-protocols" ]; then
    echo "Downloading Wayland protocols..."
    if ! git clone https://gitlab.freedesktop.org/wayland/wayland-protocols.git --depth 1; then
        echo "ERROR: Failed to download Wayland protocols"
        exit 1
    fi
fi

cd ../..

# Setup GLAD (using existing external/glad)
echo "GLAD setup complete (using existing files)"

echo "Creating build directory..."
mkdir -p build

echo "Setup complete!"
echo ""
echo "Next steps:"
echo "  1. Run 'Build Project' to compile"
echo "  2. Run 'Run Application' to test"