#include "layout_manager.h"

LayoutManager::LayoutManager(float win_width, float win_height) 
    : window_width(win_width), window_height(win_height) {
}

LayoutManager::~LayoutManager() {
    clear_all();
}

void LayoutManager::set_window_size(float width, float height) {
    window_width = width;
    window_height = height;
}

void LayoutManager::register_layout(Layout* layout) {
    if (layout) {
        layouts.push_back(layout);
    }
}

void LayoutManager::register_box(Box* box) {
    if (box) {
        boxes.push_back(box);
    }
}

void LayoutManager::handle_window_resize(float new_width, float new_height) {
    float width_ratio = new_width / window_width;
    float height_ratio = new_height / window_height;
    
    window_width = new_width;
    window_height = new_height;
    
    for (auto* layout : layouts) {
        const auto& old_area = layout->get_area();
        layout->set_area(old_area.x * width_ratio, old_area.y * height_ratio,
                        old_area.width * width_ratio, old_area.height * height_ratio);
        layout->recalculate();
    }
    
    for (auto* box : boxes) {
        const auto& old_area = box->get_area();
        BoxArea new_area;
        new_area.x = old_area.x * width_ratio;
        new_area.y = old_area.y * height_ratio;
        new_area.width = old_area.width * width_ratio;
        new_area.height = old_area.height * height_ratio;
    }
}

void LayoutManager::render_all() {
    for (auto* box : boxes) {
        box->render();
    }
}

void LayoutManager::handle_touch_for_all(const TouchData& touch_data) {
    for (auto* box : boxes) {
        box->handle_touch(touch_data);
    }
}

void LayoutManager::clear_all() {
    for (auto* layout : layouts) {
        delete layout;
    }
    for (auto* box : boxes) {
        delete box;
    }
    layouts.clear();
    boxes.clear();
}