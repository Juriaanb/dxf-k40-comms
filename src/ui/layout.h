#pragma once

#include <vector>
#include <string>
#include <memory>

struct LayoutArea {
    float x, y, width, height;
};

struct LayoutElement {
    std::string type; // "box" or "layout"
    LayoutArea area;
    void* element_ptr; // Points to Box* or Layout*
};

class Layout {
private:
    LayoutArea area;
    std::vector<std::vector<LayoutElement>> rows;
    std::vector<float> custom_row_heights;
    std::vector<std::vector<float>> custom_col_widths;
    bool is_calculated;
    
    void parse_row_string(const std::string& row_str, std::vector<std::string>& elements);
    void calculate_positions();
    
public:
    Layout();
    Layout(float x, float y, float width, float height);
    
    void set_area(float x, float y, float width, float height);
    void add_row(const std::string& row_definition);
    void set_custom_row_height(int row_index, float height);
    void set_custom_col_width(int row_index, int col_index, float width);
    
    void add_element_to_current_row(const std::string& type, void* element);
    
    void recalculate();
    LayoutArea get_element_area(int row, int col) const;
    
    const LayoutArea& get_area() const { return area; }
    bool needs_recalculation() const { return !is_calculated; }
    
    void update_from_parent_ratio(float parent_x, float parent_y, 
                                 float parent_width, float parent_height,
                                 float ratio_x, float ratio_y, 
                                 float ratio_width, float ratio_height);
};

Layout* create_layout(float x = 0, float y = 0, float width = 1.0f, float height = 1.0f);