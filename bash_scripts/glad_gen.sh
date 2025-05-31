#!/bin/bash

# Generate GLAD
# Downloads and generates GLAD OpenGL loader

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
EXTERNAL_DIR="$PROJECT_DIR/external"
GLAD_DIR="$EXTERNAL_DIR/glad"

echo "=== Generating GLAD ==="

# Create external directory
mkdir -p "$EXTERNAL_DIR"

# Check if GLAD already exists
if [ -d "$GLAD_DIR" ]; then
    echo "GLAD already exists at $GLAD_DIR"
    read -p "Regenerate? (y/n): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Skipping GLAD generation"
        exit 0
    fi
    rm -rf "$GLAD_DIR"
fi

# Download GLAD
echo "Downloading GLAD..."
cd "$EXTERNAL_DIR"

# Use wget to download pre-generated GLAD for OpenGL 4.6 Core
GLAD_URL="https://github.com/Dav1dde/glad/archive/refs/heads/master.zip"

if command -v wget &> /dev/null; then
    wget -O glad.zip "$GLAD_URL"
elif command -v curl &> /dev/null; then
    curl -L -o glad.zip "$GLAD_URL"
else
    echo "ERROR: wget or curl required to download GLAD"
    exit 1
fi

# Extract
if command -v unzip &> /dev/null; then
    unzip -q glad.zip
    mv glad-master glad
    rm glad.zip
else
    echo "ERROR: unzip required to extract GLAD"
    exit 1
fi

echo "GLAD downloaded to $GLAD_DIR"

# Note: For production, you should generate GLAD specifically for your needs
# at https://glad.dav1d.de/ with OpenGL 4.6 Core profile
echo ""
echo "NOTE: This downloads the GLAD repository."
echo "For production, generate GLAD at https://glad.dav1d.de/"
echo "with OpenGL 4.6 Core profile and replace the files."