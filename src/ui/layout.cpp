#include "layout.h"
#include <sstream>

Layout::Layout() : area{0, 0, 1.0f, 1.0f}, is_calculated(false) {
}

Layout::Layout(float x, float y, float width, float height) 
    : area{x, y, width, height}, is_calculated(false) {
}

void Layout::set_area(float x, float y, float width, float height) {
    area = {x, y, width, height};
    is_calculated = false;
}

void Layout::parse_row_string(const std::string& row_str, std::vector<std::string>& elements) {
    std::istringstream iss(row_str);
    std::string element;
    elements.clear();
    
    while (iss >> element) {
        elements.push_back(element);
    }
}

void Layout::add_row(const std::string& row_definition) {
    std::vector<std::string> element_names;
    parse_row_string(row_definition, element_names);
    
    std::vector<LayoutElement> row;
    for (const auto& name : element_names) {
        LayoutElement elem;
        elem.type = name;
        elem.element_ptr = nullptr;
        row.push_back(elem);
    }
    
    rows.push_back(row);
    custom_row_heights.push_back(-1.0f); // -1 means use automatic height
    
    // Initialize column widths for this row (-1 means automatic width)
    std::vector<float> row_col_widths(row.size(), -1.0f);
    custom_col_widths.push_back(row_col_widths);
    
    is_calculated = false;
}

void Layout::set_custom_row_height(int row_index, float height) {
    if (row_index >= 0 && row_index < static_cast<int>(custom_row_heights.size())) {
        custom_row_heights[row_index] = height;
        is_calculated = false;
    }
}

void Layout::set_custom_col_width(int row_index, int col_index, float width) {
    if (row_index >= 0 && row_index < static_cast<int>(custom_col_widths.size()) &&
        col_index >= 0 && col_index < static_cast<int>(custom_col_widths[row_index].size())) {
        custom_col_widths[row_index][col_index] = width;
        is_calculated = false;
    }
}

void Layout::add_element_to_current_row(const std::string& type, void* element) {
    if (rows.empty()) return;
    
    auto& current_row = rows.back();
    for (auto& elem : current_row) {
        if (elem.element_ptr == nullptr && elem.type == type) {
            elem.element_ptr = element;
            break;
        }
    }
}

void Layout::calculate_positions() {
    if (rows.empty()) {
        is_calculated = true;
        return;
    }
    
    // Calculate total used height by custom rows and remaining height for auto rows
    float total_custom_height = 0.0f;
    int auto_rows = 0;
    
    for (size_t i = 0; i < custom_row_heights.size(); ++i) {
        if (custom_row_heights[i] > 0) {
            total_custom_height += custom_row_heights[i];
        } else {
            auto_rows++;
        }
    }
    
    float remaining_height = area.height - total_custom_height;
    float auto_row_height = auto_rows > 0 ? remaining_height / static_cast<float>(auto_rows) : 0.0f;
    
    float current_y = area.y;
    
    for (size_t row_idx = 0; row_idx < rows.size(); ++row_idx) {
        auto& row = rows[row_idx];
        float row_height = (custom_row_heights[row_idx] > 0) ? custom_row_heights[row_idx] : auto_row_height;
        
        // Calculate column widths for this row
        float total_custom_width = 0.0f;
        int auto_cols = 0;
        
        for (size_t col_idx = 0; col_idx < row.size(); ++col_idx) {
            if (custom_col_widths[row_idx][col_idx] > 0) {
                total_custom_width += custom_col_widths[row_idx][col_idx];
            } else {
                auto_cols++;
            }
        }
        
        float remaining_width = area.width - total_custom_width;
        float auto_col_width = auto_cols > 0 ? remaining_width / static_cast<float>(auto_cols) : 0.0f;
        
        float current_x = area.x;
        
        for (size_t col_idx = 0; col_idx < row.size(); ++col_idx) {
            auto& elem = row[col_idx];
            float col_width = (custom_col_widths[row_idx][col_idx] > 0) ? 
                             custom_col_widths[row_idx][col_idx] : auto_col_width;
            
            elem.area.x = current_x;
            elem.area.y = area.height - current_y - row_height;
            elem.area.width = col_width;
            elem.area.height = row_height;
            
            current_x += col_width;
        }
        
        current_y += row_height;
    }
    
    is_calculated = true;
}

void Layout::recalculate() {
    calculate_positions();
}

LayoutArea Layout::get_element_area(int row, int col) const {
    if (row >= 0 && row < static_cast<int>(rows.size()) && 
        col >= 0 && col < static_cast<int>(rows[row].size())) {
        return rows[row][col].area;
    }
    return {0, 0, 0, 0};
}

void Layout::update_from_parent_ratio(float parent_x, float parent_y, 
                                     float parent_width, float parent_height,
                                     float ratio_x, float ratio_y, 
                                     float ratio_width, float ratio_height) {
    area.x = parent_x + ratio_x * parent_width;
    area.y = parent_y + ratio_y * parent_height;
    area.width = ratio_width * parent_width;
    area.height = ratio_height * parent_height;
    
    is_calculated = false;
    calculate_positions();
}

Layout* create_layout(float x, float y, float width, float height) {
    return new Layout(x, y, width, height);
}