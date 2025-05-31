#include "box.h"
#include <GLES2/gl2.h>
#include <iostream>

int Box::next_id = 0;

Box::Box(float x, float y, float width, float height, 
         BoxCallback cb, const std::string& txt, 
         const std::string& align, bool block,
         const Color& bg, const Color& text_col) 
    : id(next_id++), area{x, y, width, height}, text(txt), 
      text_align(align), blocking(block), callback(cb),
      bg_color(bg), text_color(text_col) {
}

void Box::handle_touch(const TouchData& touch_data) {
    if (!area.contains_point(touch_data.x, touch_data.y)) {
        return;
    }
    
    bool can_process = TouchHandler::check_and_register_touch(id, touch_data, blocking);
    
    if (can_process && callback) {
        callback(touch_data, area);
    }
}

void Box::render() {
    glScissor((int)area.x, (int)area.y, (int)area.width, (int)area.height);
    glEnable(GL_SCISSOR_TEST);
    
    glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glDisable(GL_SCISSOR_TEST);
}

Box* create_box(float x, float y, float width, float height, 
                BoxCallback callback, const std::string& text, 
                const std::string& text_align, bool blocking,
                const Color& bg_color, const Color& text_color) {
    return new Box(x, y, width, height, callback, text, text_align, blocking, bg_color, text_color);
}