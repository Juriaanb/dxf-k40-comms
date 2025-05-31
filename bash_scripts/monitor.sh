#!/bin/bash

# Monitor Logs
# Real-time monitoring of application logs

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
LOG_DIR="$PROJECT_DIR/logs"

echo "=== Log Monitor ==="

if [ ! -d "$LOG_DIR" ]; then
    echo "No logs directory found. Run the application first."
    exit 1
fi

# Find latest log file
LATEST_LOG=$(find "$LOG_DIR" -name "run_*.log" -type f -printf '%T@ %p\n' | sort -n | tail -1 | cut -d' ' -f2-)

if [ -z "$LATEST_LOG" ]; then
    echo "No log files found in $LOG_DIR"
    exit 1
fi

echo "Monitoring: $LATEST_LOG"
echo "Press Ctrl+C to stop monitoring"
echo ""

# Monitor the log file
tail -f "$LATEST_LOG"