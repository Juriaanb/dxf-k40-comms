#include "window_data.h"
#include "base_window.h"
#include "ui/layout.h"
#include "ui/box.h"
#include "ui/layout_manager.h"
#include <iostream>

// Main loop function called by window
int main_loop(const WindowData& data) {
    static bool initialized = false;
    static LayoutManager* layout_manager = nullptr;
    static Layout* main_layout = nullptr;
    static Box* test_box1 = nullptr;
    static Box* test_box2 = nullptr;
    static Box* test_box3 = nullptr;
    
    // Initialize on first call
    if (!initialized) {
        std::cout << "Initializing main loop..." << std::endl;
        layout_manager = new LayoutManager(data.screen_width, data.screen_height);
        
        // Create test layout: "box box box"
        main_layout = create_layout(0, 0, data.screen_width, data.screen_height);
        main_layout->add_row("box box box");
        main_layout->recalculate();
        
        // Create test boxes with different colors
        auto box1_area = main_layout->get_element_area(0, 0);
        auto box2_area = main_layout->get_element_area(0, 1);
        auto box3_area = main_layout->get_element_area(0, 2);
        
        std::cout << "Box 1 area: " << box1_area.x << "," << box1_area.y << " " << box1_area.width << "x" << box1_area.height << std::endl;
        std::cout << "Box 2 area: " << box2_area.x << "," << box2_area.y << " " << box2_area.width << "x" << box2_area.height << std::endl;
        std::cout << "Box 3 area: " << box3_area.x << "," << box3_area.y << " " << box3_area.width << "x" << box3_area.height << std::endl;
        
        test_box1 = create_box(box1_area.x, box1_area.y, box1_area.width, box1_area.height,
                              [](const TouchData&, const BoxArea&) { std::cout << "Box 1 clicked!" << std::endl; },
                              "Box 1", "center", true,
                              Color(0.8f, 0.2f, 0.2f), Color(1.0f, 1.0f, 1.0f));
        
        test_box2 = create_box(box2_area.x, box2_area.y, box2_area.width, box2_area.height,
                              [](const TouchData&, const BoxArea&) { std::cout << "Box 2 clicked!" << std::endl; },
                              "Box 2", "center", true,
                              Color(0.2f, 0.8f, 0.2f), Color(1.0f, 1.0f, 1.0f));
        
        test_box3 = create_box(box3_area.x, box3_area.y, box3_area.width, box3_area.height,
                              [](const TouchData&, const BoxArea&) { std::cout << "Box 3 clicked!" << std::endl; },
                              "Box 3", "center", true,
                              Color(0.2f, 0.2f, 0.8f), Color(1.0f, 1.0f, 1.0f));
        
        layout_manager->register_layout(main_layout);
        layout_manager->register_box(test_box1);
        layout_manager->register_box(test_box2);
        layout_manager->register_box(test_box3);
        
        initialized = true;
    }
    
    // Handle window resize
    if (data.window_resized) {
        layout_manager->handle_window_resize(data.screen_width, data.screen_height);
    }
    
    // Handle touch events
    if (data.mouse_pressed || data.mouse_held || data.mouse_released) {
        std::cout << "Mouse event: " << data.mouse_x << "," << data.mouse_y 
                  << " pressed=" << data.mouse_pressed << " held=" << data.mouse_held 
                  << " released=" << data.mouse_released << std::endl;
                  
        TouchData touch_data;
        touch_data.x = data.mouse_x;
        touch_data.y = data.mouse_y;
        touch_data.pressed = data.mouse_pressed;
        touch_data.held = data.mouse_held;
        touch_data.released = data.mouse_released;
        
        layout_manager->handle_touch_for_all(touch_data);
    }
    
    // Render everything
    layout_manager->render_all();
    
    // Return 0 for success, -1 for failure (which closes window)
    return data.should_exit ? -1 : 0;
}

// Entry point that creates window and starts the loop
int main(int argc, char* argv[]) {
    BaseWindow window(800, 600, "STEP Viewer");
    
    if (!window.initialize()) {
        std::cerr << "Failed to initialize window" << std::endl;
        return -1;
    }
    
    // Start window with main_loop as callback
    window.run_with_callback(main_loop);
    return 0;
}