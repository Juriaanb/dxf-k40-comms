#include "base_window.h"
#include <iostream>
#include <cstring>

// Wayland registry listeners
static const struct wl_registry_listener registry_listener = {
    BaseWindow::registry_global,
    BaseWindow::registry_global_remove
};

// XDG Shell listeners  
static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    BaseWindow::xdg_wm_base_ping
};

static const struct xdg_surface_listener xdg_surface_listener = {
    BaseWindow::xdg_surface_configure
};

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    BaseWindow::xdg_toplevel_configure,
    BaseWindow::xdg_toplevel_close
};

// Pointer listeners
static const struct wl_pointer_listener pointer_listener = {
    BaseWindow::pointer_enter,
    BaseWindow::pointer_leave,
    BaseWindow::pointer_motion,
    BaseWindow::pointer_button,
    BaseWindow::pointer_axis,
    BaseWindow::pointer_frame
};

BaseWindow* BaseWindow::current_instance = nullptr;

BaseWindow::BaseWindow(int w, int h, const std::string& t) 
    : display(nullptr), registry(nullptr), compositor(nullptr), surface(nullptr),
      egl_window(nullptr), xdg_wm_base(nullptr), xdg_surface(nullptr), 
      xdg_toplevel(nullptr), seat(nullptr), pointer(nullptr), keyboard(nullptr),
      egl_display(EGL_NO_DISPLAY), egl_context(EGL_NO_CONTEXT), 
      egl_surface(EGL_NO_SURFACE), width(w), height(h), title(t),
      configured(false), running(true), main_callback(nullptr) {
    
    window_data.screen_width = static_cast<float>(width);
    window_data.screen_height = static_cast<float>(height);
    current_instance = this;
}

BaseWindow::~BaseWindow() {
    cleanup();
}

bool BaseWindow::initialize() {
    if (!init_wayland()) {
        return false;
    }
    
    if (!init_egl()) {
        return false;
    }
    
    // Wait for initial configuration
    while (!configured && running) {
        process_events();
    }
    
    // Initialize OpenGL state
    glViewport(0, 0, width, height);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    std::cout << "Base window initialized: " << width << "x" << height << std::endl;
    std::cout << "OpenGL ES " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    
    return true;
}

void BaseWindow::run() {
    std::cout << "Starting basic main loop..." << std::endl;
    
    while (running) {
        process_events();
        
        // Update window dimensions if changed
        window_data.screen_width = static_cast<float>(width);
        window_data.screen_height = static_cast<float>(height);
        
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Swap buffers
        swap_buffers();
    }
    
    std::cout << "Main loop ended" << std::endl;
}

void BaseWindow::run_with_callback(MainLoopFunction callback) {
    std::cout << "Starting main loop with callback..." << std::endl;
    main_callback = callback;
    
    window_data.should_exit = false;
    
    while (running) {
        process_events();
        
        // Update window dimensions 
        int old_width = static_cast<int>(window_data.screen_width);
        int old_height = static_cast<int>(window_data.screen_height);
        
        window_data.screen_width = static_cast<float>(width);
        window_data.screen_height = static_cast<float>(height);
        
        if (width != old_width || height != old_height) {
            window_data.window_resized = true;
            glViewport(0, 0, width, height);
        } else {
            window_data.window_resized = false;
        }
        
        // Reset mouse events (will be updated by Wayland handlers)
        window_data.mouse_pressed = false;
        window_data.mouse_released = false;
        
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Call main loop function
        if (main_callback) {
            int result = main_callback(window_data);
            if (result != 0) {
                std::cout << "Main callback returned error, closing window" << std::endl;
                running = false;
                break;
            }
        }
        
        // Swap buffers
        swap_buffers();
    }
    
    std::cout << "Callback loop ended" << std::endl;
}

bool BaseWindow::init_wayland() {
    display = wl_display_connect(nullptr);
    if (!display) {
        std::cerr << "Failed to connect to Wayland display" << std::endl;
        return false;
    }
    
    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, this);
    
    wl_display_dispatch(display);
    wl_display_roundtrip(display);
    
    if (!compositor) {
        std::cerr << "Wayland compositor not available" << std::endl;
        return false;
    }
    
    surface = wl_compositor_create_surface(compositor);
    if (!surface) {
        std::cerr << "Failed to create Wayland surface" << std::endl;
        return false;
    }
    
    if (!xdg_wm_base) {
        std::cerr << "XDG WM Base not available" << std::endl;
        return false;
    }
    
    xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, surface);
    xdg_surface_add_listener(xdg_surface, &xdg_surface_listener, this);
    
    xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
    xdg_toplevel_add_listener(xdg_toplevel, &xdg_toplevel_listener, this);
    
    xdg_toplevel_set_title(xdg_toplevel, title.c_str());
    
    wl_surface_commit(surface);
    
    return true;
}

bool BaseWindow::init_egl() {
    egl_display = eglGetDisplay((EGLNativeDisplayType)display);
    if (egl_display == EGL_NO_DISPLAY) {
        std::cerr << "Failed to get EGL display" << std::endl;
        return false;
    }
    
    if (!eglInitialize(egl_display, nullptr, nullptr)) {
        std::cerr << "Failed to initialize EGL" << std::endl;
        return false;
    }
    
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    
    EGLint num_configs;
    if (!eglChooseConfig(egl_display, config_attribs, &egl_config, 1, &num_configs)) {
        std::cerr << "Failed to choose EGL config" << std::endl;
        return false;
    }
    
    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, context_attribs);
    if (egl_context == EGL_NO_CONTEXT) {
        std::cerr << "Failed to create EGL context" << std::endl;
        return false;
    }
    
    egl_window = wl_egl_window_create(surface, width, height);
    if (!egl_window) {
        std::cerr << "Failed to create EGL window" << std::endl;
        return false;
    }
    
    egl_surface = eglCreateWindowSurface(egl_display, egl_config, egl_window, nullptr);
    if (egl_surface == EGL_NO_SURFACE) {
        std::cerr << "Failed to create EGL surface" << std::endl;
        return false;
    }
    
    if (!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context)) {
        std::cerr << "Failed to make EGL context current" << std::endl;
        return false;
    }
    
    return true;
}

void BaseWindow::process_events() {
    wl_display_dispatch_pending(display);
    wl_display_flush(display);
}

void BaseWindow::swap_buffers() {
    eglSwapBuffers(egl_display, egl_surface);
}

void BaseWindow::cleanup() {
    if (egl_surface != EGL_NO_SURFACE) {
        eglDestroySurface(egl_display, egl_surface);
    }
    
    if (egl_context != EGL_NO_CONTEXT) {
        eglDestroyContext(egl_display, egl_context);
    }
    
    if (egl_display != EGL_NO_DISPLAY) {
        eglTerminate(egl_display);
    }
    
    if (egl_window) {
        wl_egl_window_destroy(egl_window);
    }
    
    if (pointer) {
        wl_pointer_destroy(pointer);
    }
    
    if (seat) {
        wl_seat_destroy(seat);
    }
    
    if (xdg_toplevel) {
        xdg_toplevel_destroy(xdg_toplevel);
    }
    
    if (xdg_surface) {
        xdg_surface_destroy(xdg_surface);
    }
    
    if (xdg_wm_base) {
        xdg_wm_base_destroy(xdg_wm_base);
    }
    
    if (surface) {
        wl_surface_destroy(surface);
    }
    
    if (compositor) {
        wl_compositor_destroy(compositor);
    }
    
    if (registry) {
        wl_registry_destroy(registry);
    }
    
    if (display) {
        wl_display_disconnect(display);
    }
}

// Wayland callbacks
void BaseWindow::registry_global(void* data, struct wl_registry* registry,
                                uint32_t name, const char* interface, uint32_t version) {
    BaseWindow* window = static_cast<BaseWindow*>(data);
    
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        window->compositor = static_cast<struct wl_compositor*>(
            wl_registry_bind(registry, name, &wl_compositor_interface, 4));
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        window->xdg_wm_base = static_cast<struct xdg_wm_base*>(
            wl_registry_bind(registry, name, &xdg_wm_base_interface, 1));
        xdg_wm_base_add_listener(window->xdg_wm_base, &xdg_wm_base_listener, window);
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        window->seat = static_cast<struct wl_seat*>(
            wl_registry_bind(registry, name, &wl_seat_interface, 7));
        
        // Get pointer for mouse input
        window->pointer = wl_seat_get_pointer(window->seat);
        if (window->pointer) {
            wl_pointer_add_listener(window->pointer, &pointer_listener, window);
        }
    }
}

void BaseWindow::registry_global_remove(void* data, struct wl_registry* registry, uint32_t name) {
    // Handle global removal
}

void BaseWindow::xdg_surface_configure(void* data, struct xdg_surface* surface, uint32_t serial) {
    BaseWindow* window = static_cast<BaseWindow*>(data);
    xdg_surface_ack_configure(surface, serial);
    window->configured = true;
}

void BaseWindow::xdg_toplevel_configure(void* data, struct xdg_toplevel* toplevel,
                                       int32_t width, int32_t height, struct wl_array* states) {
    BaseWindow* window = static_cast<BaseWindow*>(data);
    
    if (width > 0 && height > 0) {
        window->width = width;
        window->height = height;
        
        if (window->egl_window) {
            wl_egl_window_resize(window->egl_window, width, height, 0, 0);
        }
    }
}

void BaseWindow::xdg_toplevel_close(void* data, struct xdg_toplevel* toplevel) {
    BaseWindow* window = static_cast<BaseWindow*>(data);
    window->running = false;
}

void BaseWindow::xdg_wm_base_ping(void* data, struct xdg_wm_base* shell, uint32_t serial) {
    xdg_wm_base_pong(shell, serial);
}

// Pointer event handlers - update WindowData directly
void BaseWindow::pointer_enter(void* data, struct wl_pointer* pointer, uint32_t serial,
                              struct wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy) {
    BaseWindow* window = static_cast<BaseWindow*>(data);
    window->window_data.mouse_x = wl_fixed_to_double(sx);
    window->window_data.mouse_y = wl_fixed_to_double(sy);
}

void BaseWindow::pointer_leave(void* data, struct wl_pointer* pointer, uint32_t serial,
                              struct wl_surface* surface) {
    BaseWindow* window = static_cast<BaseWindow*>(data);
    window->window_data.mouse_held = false;
}

void BaseWindow::pointer_motion(void* data, struct wl_pointer* pointer, uint32_t time,
                               wl_fixed_t sx, wl_fixed_t sy) {
    BaseWindow* window = static_cast<BaseWindow*>(data);
    window->window_data.mouse_x = wl_fixed_to_double(sx);
    window->window_data.mouse_y = wl_fixed_to_double(sy);
}

void BaseWindow::pointer_button(void* data, struct wl_pointer* pointer, uint32_t serial,
                               uint32_t time, uint32_t button, uint32_t state) {
    BaseWindow* window = static_cast<BaseWindow*>(data);
    
    if (state == 1) { // Button pressed
        window->window_data.mouse_pressed = true;
        window->window_data.mouse_held = true;
        window->window_data.mouse_released = false;
    } else { // Button released
        window->window_data.mouse_pressed = false;
        window->window_data.mouse_held = false;
        window->window_data.mouse_released = true;
    }
}

void BaseWindow::pointer_axis(void* data, struct wl_pointer* pointer, uint32_t time,
                             uint32_t axis, wl_fixed_t value) {
    // Handle scroll events
}

void BaseWindow::pointer_frame(void* data, struct wl_pointer* pointer) {
    // Handle pointer frame events (batching)
}