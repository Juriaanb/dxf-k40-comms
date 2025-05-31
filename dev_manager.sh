#!/bin/bash

# Development Manager - Main Interface
# Navigation: Arrow keys, Enter to select, ESC to exit

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BASH_SCRIPTS_DIR="$SCRIPT_DIR/bash_scripts"

# Colors - minimal professional palette
HEADER='\033[1;37m'    # Bold white
SELECTED='\033[7m'     # Reverse video
AI_ITEM='\033[2m'      # Dim text
INFO='\033[0;37m'      # Light gray
NC='\033[0m'           # No Color

# Menu items
declare -a MENU_ITEMS=(
    "Setup Project Dependencies"
    "Build Project"
    "Run Application"
    "Monitor Logs"
    "Exit"
)

declare -a MENU_SCRIPTS=(
    "setup.sh"
    "build.sh"
    "run.sh"
    "manager_monitor"
    "exit"
)

SELECTED=0
MENU_SIZE=${#MENU_ITEMS[@]}

# Terminal setup
setup_terminal() {
    # Save terminal state
    stty -echo
    tput civis  # Hide cursor
    tput clear
}

# Restore terminal
restore_terminal() {
    stty echo
    tput cnorm  # Show cursor
    tput clear
}

# Draw header
draw_header() {
    echo -e "${HEADER}STEP VIEWER DEVELOPMENT MANAGER${NC}"
    echo -e "${INFO}────────────────────────────────────────────────────────────────${NC}"
    echo -e "${INFO}Navigation: ↑↓ Arrow Keys  │  Enter: Select  │  ESC: Exit${NC}"
    echo ""
}

# Draw menu
draw_menu() {
    tput clear
    draw_header
    
    for i in "${!MENU_ITEMS[@]}"; do
        if [ $i -eq $SELECTED ]; then
            echo -e "  \033[7m${MENU_ITEMS[$i]}${NC}"
        else
            echo -e "    ${MENU_ITEMS[$i]}"
        fi
    done
    
    echo ""
    echo -e "${INFO}Project: $SCRIPT_DIR${NC}"
    echo -e "${INFO}Scripts: $BASH_SCRIPTS_DIR${NC}"
}

# Execute selected script
execute_script() {
    local script_name="${MENU_SCRIPTS[$SELECTED]}"
    local script_path="$BASH_SCRIPTS_DIR/$script_name"
    
    if [ "$script_name" = "exit" ]; then
        return 1
    fi
    
    # Handle built-in monitor function
    if [ "$script_name" = "manager_monitor" ]; then
        restore_terminal
        
        echo "Executing: ${MENU_ITEMS[$SELECTED]}"
        echo ""
        
        LOG_DIR="$SCRIPT_DIR/logs"
        
        if [ ! -d "$LOG_DIR" ]; then
            echo "No logs directory found. Run the application first."
            echo ""
            echo "Press any key to return to menu..."
            read -n 1 -s
            setup_terminal
            return 0
        fi
        
        # Find latest log file
        LATEST_LOG=$(find "$LOG_DIR" -name "run_*.log" -type f -printf '%T@ %p\n' | sort -n | tail -1 | cut -d' ' -f2-)
        
        if [ -z "$LATEST_LOG" ]; then
            echo "No log files found in $LOG_DIR"
            echo ""
            echo "Press any key to return to menu..."
            read -n 1 -s
            setup_terminal
            return 0
        fi
        
        echo "Monitoring: $LATEST_LOG"
        echo "Press Ctrl+C to stop monitoring"
        echo ""
        
        # Monitor the log file
        tail -f "$LATEST_LOG"
        
        # This won't be reached unless tail is interrupted
        echo ""
        echo "Press any key to return to menu..."
        read -n 1 -s
        setup_terminal
        return 0
    fi
    
    restore_terminal
    
    echo "Executing: ${MENU_ITEMS[$SELECTED]}"
    echo "Script: $script_name"
    echo ""
    
    if [ -f "$script_path" ]; then
        chmod +x "$script_path"
        "$script_path"
        local exit_code=$?
        
        echo ""
        if [ $exit_code -eq 0 ]; then
            echo "Script completed successfully"
        else
            echo "Script failed with exit code $exit_code"
        fi
    else
        echo "ERROR: Script not found: $script_path"
    fi
    
    echo ""
    echo "Press any key to return to menu..."
    read -n 1 -s
    
    setup_terminal
    return 0
}

# Handle input
handle_input() {
    local key
    IFS= read -rsn1 key
    
    case "$key" in
        $'\x1b')  # ESC sequence
            IFS= read -rsn2 key
            case "$key" in
                '[A') # Up arrow
                    ((SELECTED--))
                    if [ $SELECTED -lt 0 ]; then
                        SELECTED=$((MENU_SIZE - 1))
                    fi
                    ;;
                '[B') # Down arrow
                    ((SELECTED++))
                    if [ $SELECTED -ge $MENU_SIZE ]; then
                        SELECTED=0
                    fi
                    ;;
            esac
            ;;
        '') # Enter
            execute_script
            return $?
            ;;
        $'\x1b') # ESC alone
            return 1
            ;;
    esac
    return 0
}

# Main loop
main() {
    # Check if bash_scripts directory exists
    if [ ! -d "$BASH_SCRIPTS_DIR" ]; then
        echo "ERROR: bash_scripts directory not found at $BASH_SCRIPTS_DIR"
        exit 1
    fi
    
    setup_terminal
    
    # Trap to restore terminal on exit
    trap restore_terminal EXIT INT TERM
    
    while true; do
        draw_menu
        if ! handle_input; then
            break
        fi
    done
    
    restore_terminal
    echo "Development manager exited."
}

# Run if executed directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi