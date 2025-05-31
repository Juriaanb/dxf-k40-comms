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

# Install development tools
sudo dnf install -y \
    cmake \
    gcc-c++ \
    make \
    git \
    pkg-config

# Install OpenGL and GLFW
sudo dnf install -y \
    mesa-libGL-devel \
    mesa-libGLU-devel \
    glfw-devel

# Install OpenCASCADE (for STEP files)
sudo dnf install -y \
    opencascade-devel

# Install other utilities
sudo dnf install -y \
    tree \
    htop \
    vim

echo "System dependencies installed"

# Setup GLAD
echo "Setting up GLAD..."
./bash_scripts/glad_gen.sh

echo "Creating build directory..."
mkdir -p build

echo "Setup complete!"
echo ""
echo "Next steps:"
echo "  1. Run 'Build Project' to compile"
echo "  2. Run 'Run Application' to test"