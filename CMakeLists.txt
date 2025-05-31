cmake_minimum_required(VERSION 3.16)
project(StepViewer VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(PkgConfig REQUIRED)

# Check for local Wayland installation first
set(LOCAL_WAYLAND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/wayland-local)
if(EXISTS ${LOCAL_WAYLAND_DIR}/wayland-install)
    message(STATUS "Using local Wayland installation")
    set(WAYLAND_CLIENT_INCLUDE_DIRS ${LOCAL_WAYLAND_DIR}/wayland-install/include)
    set(WAYLAND_CLIENT_LIBRARIES ${LOCAL_WAYLAND_DIR}/wayland-install/lib64/libwayland-client.so)
    set(WAYLAND_EGL_LIBRARIES ${LOCAL_WAYLAND_DIR}/wayland-install/lib64/libwayland-egl.so)
    set(WAYLAND_PROTOCOLS_DIR ${LOCAL_WAYLAND_DIR}/wayland-protocols)
    
    # Set pkg-config path for local installation
    set(ENV{PKG_CONFIG_PATH} "${LOCAL_WAYLAND_DIR}/wayland-install/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
    
    # Try pkg-config with local path
    pkg_check_modules(EGL egl)
    pkg_check_modules(GLESV2 glesv2)
    
    set(WAYLAND_FOUND TRUE)
else()
    # Fallback to system packages
    message(STATUS "Trying system Wayland packages")
    pkg_check_modules(WAYLAND_CLIENT wayland-client)
    pkg_check_modules(WAYLAND_EGL wayland-egl)
    pkg_check_modules(EGL egl)
    pkg_check_modules(GLESV2 glesv2)
    pkg_check_modules(WAYLAND_PROTOCOLS wayland-protocols)
    
    if(WAYLAND_CLIENT_FOUND)
        set(WAYLAND_FOUND TRUE)
    else()
        message(FATAL_ERROR "Wayland not found. Run setup script first: ./bash_scripts/setup.sh")
    endif()
endif()

# Generate Wayland protocol headers
if(EXISTS ${LOCAL_WAYLAND_DIR}/wayland-install)
    set(WAYLAND_SCANNER ${LOCAL_WAYLAND_DIR}/wayland-install/bin/wayland-scanner)
else()
    find_program(WAYLAND_SCANNER wayland-scanner)
endif()

if(NOT WAYLAND_SCANNER)
    message(FATAL_ERROR "wayland-scanner not found")
endif()

# Get wayland-protocols directory
if(EXISTS ${LOCAL_WAYLAND_DIR}/wayland-protocols)
    set(WAYLAND_PROTOCOLS_DIR ${LOCAL_WAYLAND_DIR}/wayland-protocols)
else()
    pkg_get_variable(WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
endif()

# Generate XDG Shell protocol
set(XDG_SHELL_PROTOCOL ${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-client-protocol.h
    COMMAND ${WAYLAND_SCANNER} client-header ${XDG_SHELL_PROTOCOL} ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-client-protocol.h
    DEPENDS ${XDG_SHELL_PROTOCOL}
)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c
    COMMAND ${WAYLAND_SCANNER} private-code ${XDG_SHELL_PROTOCOL} ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c
    DEPENDS ${XDG_SHELL_PROTOCOL}
)

# Create protocol library
add_library(wayland-protocols STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c
)
target_include_directories(wayland-protocols PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${WAYLAND_CLIENT_INCLUDE_DIRS}
)
set_target_properties(wayland-protocols PROPERTIES LINKER_LANGUAGE C)

# Source files
set(SOURCES
    src/main.cpp
    src/base_window.cpp
    src/ui/touch_handler.cpp
    src/ui/box.cpp
    src/ui/ui_helpers.cpp
    src/ui/layout.cpp
    src/ui/layout_manager.cpp
)

set(HEADERS
    src/base_window.h
    src/window_data.h
    src/ui/touch_handler.h
    src/ui/box.h
    src/ui/ui_helpers.h
    src/ui/layout.h
    src/ui/layout_manager.h
)

# Executable
add_executable(${PROJECT_NAME} 
    ${SOURCES} 
    ${HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-client-protocol.h
)

# Make sure protocol headers are generated first
add_dependencies(${PROJECT_NAME} wayland-protocols)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    ${WAYLAND_CLIENT_LIBRARIES}
    ${WAYLAND_EGL_LIBRARIES}
    ${EGL_LIBRARIES}
    ${GLESV2_LIBRARIES}
    wayland-protocols
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    src
    src/ui
    ${CMAKE_CURRENT_BINARY_DIR}
    ${WAYLAND_CLIENT_INCLUDE_DIRS}
    ${WAYLAND_EGL_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${GLESV2_INCLUDE_DIRS}
)

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE ${WAYLAND_CLIENT_CFLAGS_OTHER})
target_compile_options(${PROJECT_NAME} PRIVATE ${WAYLAND_EGL_CFLAGS_OTHER})
target_compile_options(${PROJECT_NAME} PRIVATE ${EGL_CFLAGS_OTHER})
target_compile_options(${PROJECT_NAME} PRIVATE ${GLESV2_CFLAGS_OTHER})