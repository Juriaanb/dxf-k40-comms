#!/bin/bash

# AI: Diagnostics (30s limit)
# AI-accessible diagnostic script with timeout restriction

echo "=== AI Diagnostics (30 second limit) ==="

# Set timeout
timeout 30s bash -c '
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
    BUILD_DIR="$PROJECT_DIR/build"
    LOG_DIR="$PROJECT_DIR/logs"
    
    echo "System Info:"
    echo "  OS: $(cat /etc/fedora-release 2>/dev/null || echo "Unknown")"
    echo "  Kernel: $(uname -r)"
    echo "  CPU: $(nproc) cores"
    echo "  Memory: $(free -h | grep Mem | awk "{print \$2}")"
    
    echo ""
    echo "Project Status:"
    echo "  Project: $PROJECT_DIR"
    echo "  Build exists: $([ -d "$BUILD_DIR" ] && echo "Yes" || echo "No")"
    echo "  Binary exists: $([ -f "$BUILD_DIR/StepViewer" ] && echo "Yes" || echo "No")"
    echo "  Logs exist: $([ -d "$LOG_DIR" ] && echo "Yes" || echo "No")"
    
    echo ""
    echo "Dependencies:"
    echo "  cmake: $(command -v cmake &>/dev/null && echo "OK" || echo "Missing")"
    echo "  make: $(command -v make &>/dev/null && echo "OK" || echo "Missing")"
    echo "  g++: $(command -v g++ &>/dev/null && echo "OK" || echo "Missing")"
    echo "  GLFW: $(pkg-config --exists glfw3 && echo "OK" || echo "Missing")"
    
    if [ -d "$BUILD_DIR" ]; then
        echo ""
        echo "Build Directory Contents:"
        ls -la "$BUILD_DIR" | head -10
    fi
    
    if [ -d "$LOG_DIR" ]; then
        echo ""
        echo "Recent Logs:"
        find "$LOG_DIR" -name "*.log" -type f -printf "%T@ %p\n" | sort -n | tail -3 | cut -d" " -f2- | while read log; do
            echo "  $(basename "$log")"
        done
    fi
    
    # Check for common issues
    echo ""
    echo "Common Issues Check:"
    
    # Check display
    if [ -z "$DISPLAY" ]; then
        echo "  WARNING: DISPLAY not set (may affect GUI)"
    else
        echo "  Display: $DISPLAY"
    fi
    
    # Check OpenGL
    if command -v glxinfo &>/dev/null; then
        GL_VERSION=$(glxinfo | grep "OpenGL version" | head -1)
        echo "  $GL_VERSION"
    else
        echo "  OpenGL info: glxinfo not available"
    fi
'

EXIT_CODE=$?

if [ $EXIT_CODE -eq 124 ]; then
    echo ""
    echo "TIMEOUT: Diagnostics exceeded 30 second limit"
elif [ $EXIT_CODE -eq 0 ]; then
    echo ""
    echo "Diagnostics completed within time limit"
else
    echo ""
    echo "Diagnostics failed with exit code: $EXIT_CODE"
fi