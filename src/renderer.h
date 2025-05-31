#pragma once
#include <glad/glad.h>
#include <string>

class Window;

class Renderer {
private:
    const Window* window;
    
    // Shader programs
    GLuint ui_shader_program;
    GLuint text_shader_program;
    
    // UI rendering
    GLuint ui_vao, ui_vbo;
    
    bool loadShader(const std::string& vertex_source, const std::string& fragment_source, GLuint& program);
    void setupUIBuffers();
    
public:
    Renderer(const Window* win);
    ~Renderer();
    
    bool init();
    void clear(float r = 0.2f, float g = 0.2f, float b = 0.2f, float a = 1.0f);
    
    // UI drawing functions (all coordinates normalized 0-1)
    void drawRect(float x, float y, float width, float height, 
                  float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
    void drawBorder(float x, float y, float width, float height, float thickness,
                   float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
    void drawText(const std::string& text, float x, float y, float scale = 1.0f,
                  float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
    
    // Custom window decorations
    void drawTitleBar();
    void drawCloseButton();
    void drawFPSCounter();
};