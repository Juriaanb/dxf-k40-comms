#!/bin/bash

# AI: Quick Build & Run (30s limit)
# AI-accessible script with timeout restriction

echo "=== AI Build & Run (30 second limit) ==="
echo "This script will timeout after 30 seconds"

# Set timeout
timeout 30s bash -c '
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
    BUILD_DIR="$PROJECT_DIR/build"
    
    echo "Building..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Quick configure and build
    if cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j$(nproc); then
        echo "Build successful, attempting to run..."
        if [ -f "./StepViewer" ]; then
            echo "Starting application..."
            ./StepViewer &
            APP_PID=$!
            sleep 5  # Let it run for 5 seconds
            kill $APP_PID 2>/dev/null
            echo "Application test completed"
        else
            echo "ERROR: Binary not found"
        fi
    else
        echo "ERROR: Build failed"
    fi
'

EXIT_CODE=$?

if [ $EXIT_CODE -eq 124 ]; then
    echo ""
    echo "TIMEOUT: Script exceeded 30 second limit"
elif [ $EXIT_CODE -eq 0 ]; then
    echo ""
    echo "Script completed within time limit"
else
    echo ""
    echo "Script failed with exit code: $EXIT_CODE"
fi