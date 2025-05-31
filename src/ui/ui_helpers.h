#pragma once

#include <string>

namespace UIHelpers {
    float calculate_ratio(float value, float min_val, float max_val);
    float ratio_to_value(float ratio, float min_val, float max_val);
    
    float distance_between_points(float x1, float y1, float x2, float y2);
    
    bool point_in_circle(float px, float py, float cx, float cy, float radius);
    
    float normalize_coordinate(float coord, float min_bound, float max_bound);
    
    struct Position {
        float x, y;
    };
    
    Position get_text_position(const std::string& align, float box_x, float box_y, 
                              float box_width, float box_height);
}