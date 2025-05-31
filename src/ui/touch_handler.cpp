#include "touch_handler.h"

bool TouchHandler::something_active = false;
int TouchHandler::active_box_id = -1;

bool TouchHandler::check_and_register_touch(int box_id, const TouchData& touch_data, bool blocking) {
    if (touch_data.released) {
        if (active_box_id == box_id) {
            release_touch(box_id);
        }
        return false;
    }
    
    if (something_active && active_box_id != box_id) {
        return false;
    }
    
    if (touch_data.pressed || touch_data.held) {
        if (blocking) {
            something_active = true;
            active_box_id = box_id;
        }
        return true;
    }
    
    return false;
}

void TouchHandler::release_touch(int box_id) {
    if (active_box_id == box_id) {
        something_active = false;
        active_box_id = -1;
    }
}

void TouchHandler::reset() {
    something_active = false;
    active_box_id = -1;
}

bool TouchHandler::is_something_active() {
    return something_active;
}