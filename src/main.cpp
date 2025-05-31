#include "base_window.h"
#include <iostream>
#include <GLES2/gl2.h>

// Test program that the base window will host
void test_program_init() {
    std::cout << "Test program initialized" << std::endl;
}

void test_program_loop() {
    // Simple test - draw a colored triangle in the center
    static float rotation = 0.0f;
    rotation += 0.01f;
    
    // This is a placeholder for actual program rendering
    // Just change background color slightly to show it's running
    float r = 0.2f + 0.1f * sin(rotation);
    float g = 0.2f + 0.1f * cos(rotation);
    float b = 0.2f;
    
    glClearColor(r, g, b, 1.0f);
}

void test_program_cleanup() {
    std::cout << "Test program cleaned up" << std::endl;
}

int main(int argc, char* argv[]) {
    // Create base window
    BaseWindow window(800, 600, "STEP Viewer - Base Window");
    
    // Set up test program
    window.set_program_init(test_program_init);
    window.set_program_loop(test_program_loop);
    window.set_program_cleanup(test_program_cleanup);
    
    // Initialize
    if (!window.initialize()) {
        std::cerr << "Failed to initialize base window" << std::endl;
        return -1;
    }
    
    // Run
    window.run();
    
    std::cout << "Application ended" << std::endl;
    return 0;
}