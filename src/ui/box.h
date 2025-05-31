#pragma once

#include "touch_handler.h"
#include <string>
#include <functional>

struct BoxArea {
    float x, y, width, height;
    
    bool contains_point(float px, float py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
};

struct Color {
    float r, g, b, a;
    Color(float red = 0.4f, float green = 0.4f, float blue = 0.4f, float alpha = 1.0f) 
        : r(red), g(green), b(blue), a(alpha) {}
};

typedef std::function<void(const TouchData&, const BoxArea&)> BoxCallback;

class Box {
private:
    static int next_id;
    
    int id;
    BoxArea area;
    std::string text;
    std::string text_align;
    bool blocking;
    BoxCallback callback;
    Color bg_color;
    Color text_color;
    
public:
    Box(float x, float y, float width, float height, 
        BoxCallback cb, const std::string& txt = "", 
        const std::string& align = "center", bool block = true,
        const Color& bg = Color(), const Color& text_col = Color(1.0f, 1.0f, 1.0f, 1.0f));
    
    void handle_touch(const TouchData& touch_data);
    void render();
    
    const BoxArea& get_area() const { return area; }
    int get_id() const { return id; }
    void set_area(const BoxArea& new_area) { area = new_area; }
};

Box* create_box(float x, float y, float width, float height, 
                BoxCallback callback, const std::string& text = "", 
                const std::string& text_align = "center", bool blocking = true,
                const Color& bg_color = Color(), const Color& text_color = Color(1.0f, 1.0f, 1.0f, 1.0f));