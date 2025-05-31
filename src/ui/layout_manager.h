#pragma once

#include "layout.h"
#include "box.h"
#include <vector>
#include <memory>

class LayoutManager {
private:
    std::vector<Layout*> layouts;
    std::vector<Box*> boxes;
    float window_width, window_height;
    
public:
    LayoutManager(float win_width, float win_height);
    ~LayoutManager();
    
    void set_window_size(float width, float height);
    void register_layout(Layout* layout);
    void register_box(Box* box);
    
    void handle_window_resize(float new_width, float new_height);
    void render_all();
    void handle_touch_for_all(const TouchData& touch_data);
    
    void clear_all();
};