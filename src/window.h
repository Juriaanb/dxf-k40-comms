#pragma once
#include <GLFW/glfw3.h>
#include <string>

class Window {
private:
    GLFWwindow* window;
    int width, height;
    float aspect_ratio;
    std::string title;
    
    // Performance tracking
    double last_time;
    int frame_count;
    double fps;
    
    // Callbacks
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void window_size_callback(GLFWwindow* window, int width, int height);
    
public:
    Window(int w, int h, const std::string& title);
    ~Window();
    
    bool init();
    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    void updateFPS();
    
    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    float getAspectRatio() const { return aspect_ratio; }
    double getFPS() const { return fps; }
    GLFWwindow* getHandle() const { return window; }
    
    // Coordinate conversion (normalized 0-1 to screen pixels)
    float normalizedToPixelX(float nx) const { return nx * width; }
    float normalizedToPixelY(float ny) const { return ny * height; }
    float pixelToNormalizedX(float px) const { return px / width; }
    float pixelToNormalizedY(float py) const { return py / height; }
    
private:
    void updateDimensions(int w, int h);
};