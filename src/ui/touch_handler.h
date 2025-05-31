#pragma once

struct TouchData {
    float x, y;
    bool pressed;
    bool released;
    bool held;
};

class TouchHandler {
private:
    static bool something_active;
    static int active_box_id;
    
public:
    static bool check_and_register_touch(int box_id, const TouchData& touch_data, bool blocking = true);
    static void release_touch(int box_id);
    static void reset();
    static bool is_something_active();
};