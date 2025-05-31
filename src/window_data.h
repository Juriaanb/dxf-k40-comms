#pragma once

struct WindowData {
    // Screen information
    float screen_width;
    float screen_height;
    
    // Mouse/touch information
    float mouse_x;
    float mouse_y;
    bool mouse_pressed;
    bool mouse_released;
    bool mouse_held;
    
    // Window state
    bool window_resized;
    bool should_exit;
    
    WindowData() : screen_width(0), screen_height(0), 
                   mouse_x(0), mouse_y(0), 
                   mouse_pressed(false), mouse_released(false), mouse_held(false),
                   window_resized(false), should_exit(false) {}
};

// Main function signature that window will call
typedef int (*MainLoopFunction)(const WindowData& data);