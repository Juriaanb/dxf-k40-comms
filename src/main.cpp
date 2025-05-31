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
        
        // Calculate scaled height for top bar (40px baseline for 1080p)
        float scale_factor = data.screen_height / 1080.0f;
        float top_bar_height = 40.0f * scale_factor;
        float resize_border_width = 8.0f; // 8px resize border
        
        // Create layout: top row "box box" (title + close), bottom row "box" (main content)
        main_layout = create_layout(0, 0, data.screen_width, data.screen_height);
        main_layout->add_row("titlebar closebtn");
        main_layout->add_row("maincontent");
        main_layout->set_custom_row_height(0, top_bar_height);
        main_layout->set_custom_col_width(0, 1, 70.0f); // Close button 70px wide
        main_layout->recalculate();
        
        // Get areas for each element
        auto titlebar_area = main_layout->get_element_area(0, 0);
        auto closebtn_area = main_layout->get_element_area(0, 1);
        auto maincontent_area = main_layout->get_element_area(1, 0);
        
        std::cout << "Title bar area: " << titlebar_area.x << "," << titlebar_area.y << " " << titlebar_area.width << "x" << titlebar_area.height << std::endl;
        std::cout << "Close button area: " << closebtn_area.x << "," << closebtn_area.y << " " << closebtn_area.width << "x" << closebtn_area.height << std::endl;
        std::cout << "Main content area: " << maincontent_area.x << "," << maincontent_area.y << " " << maincontent_area.width << "x" << maincontent_area.height << std::endl;
        
        test_box1 = create_box(titlebar_area.x, titlebar_area.y, titlebar_area.width, titlebar_area.height,
                              [](const TouchData&, const BoxArea&) { std::cout << "Title bar clicked!" << std::endl; },
                              "STEP Viewer", "center", true,
                              Color(0.3f, 0.3f, 0.3f), Color(1.0f, 1.0f, 1.0f));
        
        test_box2 = create_box(closebtn_area.x, closebtn_area.y, closebtn_area.width, closebtn_area.height,
                              [](const TouchData&, const BoxArea&) { std::cout << "Close button clicked!" << std::endl; },
                              "X", "center", true,
                              Color(0.8f, 0.2f, 0.2f), Color(1.0f, 1.0f, 1.0f));
        
        test_box3 = create_box(maincontent_area.x, maincontent_area.y, maincontent_area.width, maincontent_area.height,
                              [](const TouchData&, const BoxArea&) { std::cout << "Main content clicked!" << std::endl; },
                              "Main Content Area", "center", true,
                              Color(0.2f, 0.2f, 0.2f), Color(1.0f, 1.0f, 1.0f));
        
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
    
    // Check for resize zones
    static bool in_resize_zone = false;
    static std::string resize_direction = "";
    
    float resize_border = 15.0f; // Thicker resize area (extends into content)
    bool mouse_in_resize = false;
    std::string current_resize_dir = "";
    
    // Check edges for resize zones (centered on window border, extending inward)
    if (data.mouse_x <= resize_border) {
        if (data.mouse_y <= resize_border) {
            current_resize_dir = "nw"; // Northwest
        } else if (data.mouse_y >= data.screen_height - resize_border) {
            current_resize_dir = "sw"; // Southwest  
        } else {
            current_resize_dir = "w"; // West
        }
        mouse_in_resize = true;
    } else if (data.mouse_x >= data.screen_width - resize_border) {
        if (data.mouse_y <= resize_border) {
            current_resize_dir = "ne"; // Northeast
        } else if (data.mouse_y >= data.screen_height - resize_border) {
            current_resize_dir = "se"; // Southeast
        } else {
            current_resize_dir = "e"; // East
        }
        mouse_in_resize = true;
    } else if (data.mouse_y <= resize_border) {
        current_resize_dir = "n"; // North
        mouse_in_resize = true;
    } else if (data.mouse_y >= data.screen_height - resize_border) {
        current_resize_dir = "s"; // South
        mouse_in_resize = true;
    }
    
    // Update resize state and cursor
    if (mouse_in_resize && current_resize_dir != resize_direction) {
        resize_direction = current_resize_dir;
        std::cout << "Entering resize zone: " << resize_direction << std::endl;
        
        // Set appropriate resize cursor
        BaseWindow* window = BaseWindow::get_current_instance();
        if (window) {
            if (resize_direction == "nw" || resize_direction == "se") {
                window->set_cursor("nw-resize");
            } else if (resize_direction == "ne" || resize_direction == "sw") {
                window->set_cursor("ne-resize");
            } else if (resize_direction == "n" || resize_direction == "s") {
                window->set_cursor("ns-resize");
            } else if (resize_direction == "e" || resize_direction == "w") {
                window->set_cursor("ew-resize");
            }
        }
    } else if (!mouse_in_resize && in_resize_zone) {
        std::cout << "Leaving resize zone" << std::endl;
        resize_direction = "";
        
        // Reset to default cursor
        BaseWindow* window = BaseWindow::get_current_instance();
        if (window) {
            window->set_cursor("default");
        }
    }
    in_resize_zone = mouse_in_resize;
    
    // Handle resize interaction
    static bool resize_started = false;
    static bool was_held = false;
    
    // Check for button press transition (not held -> held) in resize zone
    bool button_just_pressed = data.mouse_held && !was_held;
    if (button_just_pressed && !resize_direction.empty() && !resize_started) {
        BaseWindow* window = BaseWindow::get_current_instance();
        if (window) {
            std::cout << "STARTING RESIZE: " << resize_direction << std::endl;
            window->start_interactive_resize(resize_direction);
            resize_started = true;
        }
    }
    
    if (data.mouse_released) {
        resize_started = false;
    }
    was_held = data.mouse_held;
    
    // Handle touch events (only if not in resize zone)
    if (!in_resize_zone && (data.mouse_pressed || data.mouse_held || data.mouse_released)) {
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