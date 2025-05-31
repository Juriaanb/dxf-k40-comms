#include "base_window.h"
#include <GLES2/gl2.h>
#include <iostream>

BaseWindow::BaseWindow(int w, int h, const std::string& t) 
    : width(w), height(h), title(t) {
}

BaseWindow::~BaseWindow() {
}

bool BaseWindow::initialize() {
    if (!wayland.initialize(width, height, title.c_str())) {
        return false;
    }
    
    // Wait for initial configuration
    while (!wayland.is_configured() && !wayland.should_close()) {
        wayland.process_events();
    }
    
    // Initialize OpenGL state
    glViewport(0, 0, width, height);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    std::cout << "Base window initialized: " << width << "x" << height << std::endl;
    std::cout << "OpenGL ES " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    
    return true;
}

void BaseWindow::run() {
    std::cout << "Starting main loop..." << std::endl;
    
    while (!wayland.should_close()) {
        // Process Wayland events
        wayland.process_events();
        
        // Update window dimensions if changed
        int new_width = wayland.get_width();
        int new_height = wayland.get_height();
        
        if (new_width != width || new_height != height) {
            width = new_width;
            height = new_height;
            glViewport(0, 0, width, height);
        }
        
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        // Swap buffers
        wayland.swap_buffers();
    }
    
    std::cout << "Main loop ended" << std::endl;
}

