#!/bin/bash

# Run Application
# Executes the STEP Viewer with logging

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"
LOG_DIR="$PROJECT_DIR/logs"
BINARY="$BUILD_DIR/StepViewer"

echo "=== Running STEP Viewer ==="

# Check if binary exists
if [ ! -f "$BINARY" ]; then
    echo "ERROR: Binary not found. Run 'Build Project' first."
    exit 1
fi

# Create logs directory
mkdir -p "$LOG_DIR"

# Generate log filename with timestamp
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
LOG_FILE="$LOG_DIR/run_$TIMESTAMP.log"

echo "Binary: $BINARY"
echo "Log: $LOG_FILE"
echo ""

# Run with logging
echo "Starting application..."
echo "   Press Ctrl+C to stop"
echo ""

# Log startup info
{
    echo "=== STEP Viewer Run Log ==="
    echo "Timestamp: $(date)"
    echo "Binary: $BINARY"
    echo "Working Directory: $(pwd)"
    echo "Environment:"
    echo "  DISPLAY=$DISPLAY"
    echo "  User: $USER"
    echo "  Shell: $SHELL"
    echo "=========================="
    echo ""
} > "$LOG_FILE"

# Run the application and capture output
"$BINARY" 2>&1 | tee -a "$LOG_FILE"
EXIT_CODE=${PIPESTATUS[0]}

echo ""
echo "Application exited with code: $EXIT_CODE"

# Log exit info
{
    echo ""
    echo "=========================="
    echo "Exit Code: $EXIT_CODE"
    echo "End Time: $(date)"
    echo "=========================="
} >> "$LOG_FILE"

if [ $EXIT_CODE -eq 0 ]; then
    echo "Application ran successfully"
else
    echo "ERROR: Application failed"
    echo "Check log: $LOG_FILE"
fi