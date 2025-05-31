#include "window.h"
#include <glad/glad.h>
#include <iostream>

Window::Window(int w, int h, const std::string& title) 
    : window(nullptr), width(w), height(h), title(title), 
      last_time(0.0), frame_count(0), fps(0.0) {
    updateDimensions(w, h);
}

Window::~Window() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool Window::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // OpenGL version 4.6 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // High refresh rate
    glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
    
    // No window decorations - we'll make our own
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    
    // Load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    // Set callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    
    // Enable VSync for smooth rendering
    glfwSwapInterval(0); // 0 = no VSync for max FPS, 1 = VSync
    
    // Initialize OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    last_time = glfwGetTime();
    
    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
    
    return true;
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::updateFPS() {
    double current_time = glfwGetTime();
    frame_count++;
    
    if (current_time - last_time >= 1.0) {
        fps = double(frame_count) / (current_time - last_time);
        frame_count = 0;
        last_time = current_time;
    }
}

void Window::updateDimensions(int w, int h) {
    width = w;
    height = h;
    aspect_ratio = float(width) / float(height);
    
    if (window) {
        glViewport(0, 0, width, height);
    }
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    win->updateDimensions(width, height);
}

void Window::window_size_callback(GLFWwindow* window, int width, int height) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    win->updateDimensions(width, height);
}