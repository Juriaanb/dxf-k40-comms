#pragma once

#include "wayland_manager.h"
#include <string>

class BaseWindow {
private:
    WaylandManager wayland;
    int width, height;
    std::string title;
    
    
public:
    BaseWindow(int w, int h, const std::string& title);
    ~BaseWindow();
    
    bool initialize();
    void run();
    
    // Window properties
    int get_width() const { return width; }
    int get_height() const { return height; }
    bool should_close() const { return wayland.should_close(); }
};