#include "base_window.h"
#include <GLES2/gl2.h>
#include <iostream>

BaseWindow::BaseWindow(int w, int h, const std::string& t) 
    : width(w), height(h), title(t), close_button_hovered(false) {
    
    // Position close button in top-right corner
    close_button_size = 20.0f;
    close_button_x = width - close_button_size - 10.0f;
    close_button_y = 10.0f;
}

BaseWindow::~BaseWindow() {
    if (program_cleanup) {
        program_cleanup();
    }
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
    
    // Run program initialization if set
    if (program_init) {
        program_init();
    }
    
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
            
            // Update close button position
            close_button_x = width - close_button_size - 10.0f;
        }
        
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Run hosted program loop if set
        if (program_loop) {
            program_loop();
        }
        
        // Render window decorations (close button)
        render_close_button();
        
        // Swap buffers
        wayland.swap_buffers();
    }
    
    std::cout << "Main loop ended" << std::endl;
}

void BaseWindow::render_close_button() {
    // Simple close button - red square with X
    // This is a minimal implementation using immediate mode style
    
    float x = close_button_x / width * 2.0f - 1.0f;
    float y = 1.0f - close_button_y / height * 2.0f;
    float size = close_button_size / width * 2.0f;
    
    // Set color - red if hovered, gray otherwise
    if (close_button_hovered) {
        glColor4f(0.8f, 0.2f, 0.2f, 0.9f);
    } else {
        glColor4f(0.6f, 0.6f, 0.6f, 0.7f);
    }
    
    // For now, just clear a small area to show where button would be
    // TODO: Implement proper button rendering with shaders
    
    // This is a placeholder - we'll implement proper rendering later
    glScissor((int)close_button_x, height - (int)close_button_y - (int)close_button_size, 
              (int)close_button_size, (int)close_button_size);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(0.6f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

bool BaseWindow::is_point_in_close_button(float x, float y) {
    return (x >= close_button_x && x <= close_button_x + close_button_size &&
            y >= close_button_y && y <= close_button_y + close_button_size);
}

void BaseWindow::set_program_init(std::function<void()> init_func) {
    program_init = init_func;
}

void BaseWindow::set_program_loop(std::function<void()> loop_func) {
    program_loop = loop_func;
}

void BaseWindow::set_program_cleanup(std::function<void()> cleanup_func) {
    program_cleanup = cleanup_func;
}