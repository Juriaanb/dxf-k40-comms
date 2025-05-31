#include "window.h"
#include "renderer.h"
#include <iostream>

int main() {
    Window window(1200, 800, "STEP Viewer");
    
    if (!window.init()) {
        std::cerr << "Failed to initialize window" << std::endl;
        return -1;
    }
    
    Renderer renderer(&window);
    if (!renderer.init()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    std::cout << "Starting main loop..." << std::endl;
    
    while (!window.shouldClose()) {
        window.pollEvents();
        window.updateFPS();
        
        // Clear screen
        renderer.clear(0.15f, 0.15f, 0.15f, 1.0f);
        
        // Draw custom window decorations
        renderer.drawTitleBar();
        renderer.drawCloseButton();
        renderer.drawFPSCounter();
        
        // Test UI elements - remove these later
        renderer.drawRect(0.1f, 0.1f, 0.8f, 0.8f, 0.3f, 0.3f, 0.3f, 0.8f); // Main area
        renderer.drawBorder(0.1f, 0.1f, 0.8f, 0.8f, 0.002f, 0.6f, 0.6f, 0.6f, 1.0f);
        
        window.swapBuffers();
    }
    
    return 0;
}