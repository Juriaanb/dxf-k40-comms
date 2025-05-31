#pragma once

#include "wayland_manager.h"
#include <string>
#include <functional>

class BaseWindow {
private:
    WaylandManager wayland;
    int width, height;
    std::string title;
    
    // Program hosting
    std::function<void()> program_loop;
    std::function<void()> program_init;
    std::function<void()> program_cleanup;
    
    // Close button state
    float close_button_x, close_button_y;
    float close_button_size;
    bool close_button_hovered;
    
    void render_close_button();
    bool is_point_in_close_button(float x, float y);
    
public:
    BaseWindow(int w, int h, const std::string& title);
    ~BaseWindow();
    
    bool initialize();
    void run();
    
    // Program hosting interface
    void set_program_init(std::function<void()> init_func);
    void set_program_loop(std::function<void()> loop_func);
    void set_program_cleanup(std::function<void()> cleanup_func);
    
    // Window properties
    int get_width() const { return width; }
    int get_height() const { return height; }
    bool should_close() const { return wayland.should_close(); }
    
    // Event forwarding for programs
    void process_events() { wayland.process_events(); }
};