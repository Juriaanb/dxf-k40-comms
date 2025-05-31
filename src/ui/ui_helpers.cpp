#include "ui_helpers.h"
#include <cmath>

namespace UIHelpers {
    
    float calculate_ratio(float value, float min_val, float max_val) {
        if (max_val == min_val) return 0.0f;
        return (value - min_val) / (max_val - min_val);
    }
    
    float ratio_to_value(float ratio, float min_val, float max_val) {
        return min_val + ratio * (max_val - min_val);
    }
    
    float distance_between_points(float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }
    
    bool point_in_circle(float px, float py, float cx, float cy, float radius) {
        return distance_between_points(px, py, cx, cy) <= radius;
    }
    
    float normalize_coordinate(float coord, float min_bound, float max_bound) {
        if (coord < min_bound) return min_bound;
        if (coord > max_bound) return max_bound;
        return coord;
    }
    
    Position get_text_position(const std::string& align, float box_x, float box_y, 
                              float box_width, float box_height) {
        Position pos;
        
        if (align == "center") {
            pos.x = box_x + box_width / 2.0f;
            pos.y = box_y + box_height / 2.0f;
        } else if (align == "left") {
            pos.x = box_x + 10.0f;
            pos.y = box_y + box_height / 2.0f;
        } else if (align == "right") {
            pos.x = box_x + box_width - 10.0f;
            pos.y = box_y + box_height / 2.0f;
        } else if (align == "top") {
            pos.x = box_x + box_width / 2.0f;
            pos.y = box_y + 10.0f;
        } else if (align == "bottom") {
            pos.x = box_x + box_width / 2.0f;
            pos.y = box_y + box_height - 10.0f;
        } else {
            pos.x = box_x + box_width / 2.0f;
            pos.y = box_y + box_height / 2.0f;
        }
        
        return pos;
    }
}