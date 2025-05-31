# STEP Viewer - Development Manager

## Quick Start

```bash
./dev_manager.sh
```

Use arrow keys to navigate, Enter to select, ESC to exit.

## Development Scripts

All development operations are controlled through the manager interface:

### Available Scripts

- **Setup Project Dependencies** - Install system packages and setup GLAD
- **Build Project** - Compile the application with CMake
- **Run Application** - Execute with logging to logs/ directory
- **Monitor Logs** - Real-time log file monitoring
- **Clean Build** - Remove build artifacts
- **Generate GLAD** - Download/setup OpenGL function loader
- **Check Dependencies** - Verify all requirements are met

### AI-Restricted Scripts (30s timeout)

- **AI: Quick Build & Run** - Limited build and test for AI diagnostics
- **AI: Diagnostics** - System and project status check for AI

### Project Structure

```
dxf-k40-comms/
├── dev_manager.sh          # Main development interface
├── bash_scripts/           # Development automation scripts
│   ├── setup.sh           # Dependency installation
│   ├── build.sh           # Project compilation
│   ├── run.sh             # Application execution with logging
│   ├── monitor.sh         # Log monitoring
│   ├── clean.sh           # Build cleanup
│   ├── glad_gen.sh        # GLAD setup
│   ├── check_deps.sh      # Dependency verification
│   ├── ai_build_run.sh    # AI-restricted build/run (30s limit)
│   └── ai_diagnostics.sh  # AI-restricted diagnostics (30s limit)
├── src/                   # Source code
├── build/                 # Build output (generated)
├── logs/                  # Application logs (generated)
└── external/              # External dependencies (generated)
```

## Requirements

- Fedora Linux
- CMake 3.16+
- GCC with C++20 support
- OpenGL 4.6+ compatible GPU
- GLFW 3.3+
- OpenCASCADE (for STEP file support)

## Usage

1. Run `./dev_manager.sh`
2. Select "Setup Project Dependencies" (first time only)
3. Select "Build Project" to compile
4. Select "Run Application" to test

All output is logged to timestamped files in the logs/ directory.