#include "wayland_manager.h"
#include <iostream>
#include <cstring>

// Wayland registry listeners
static const struct wl_registry_listener registry_listener = {
    WaylandManager::registry_global,
    WaylandManager::registry_global_remove
};

// XDG Shell listeners  
static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    WaylandManager::xdg_wm_base_ping
};

static const struct xdg_surface_listener xdg_surface_listener = {
    WaylandManager::xdg_surface_configure
};

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    WaylandManager::xdg_toplevel_configure,
    WaylandManager::xdg_toplevel_close
};

// Pointer listeners
static const struct wl_pointer_listener pointer_listener = {
    WaylandManager::pointer_enter,
    WaylandManager::pointer_leave,
    WaylandManager::pointer_motion,
    WaylandManager::pointer_button,
    WaylandManager::pointer_axis
};

WaylandManager::WaylandManager() 
    : display(nullptr), registry(nullptr), compositor(nullptr), surface(nullptr),
      egl_window(nullptr), xdg_wm_base(nullptr), xdg_surface(nullptr), 
      xdg_toplevel(nullptr), seat(nullptr), pointer(nullptr), keyboard(nullptr),
      egl_display(EGL_NO_DISPLAY), egl_context(EGL_NO_CONTEXT), 
      egl_surface(EGL_NO_SURFACE), width(800), height(600), 
      configured(false), running(true) {
}

WaylandManager::~WaylandManager() {
    cleanup();
}

bool WaylandManager::initialize(int w, int h, const char* title) {
    width = w;
    height = h;
    
    if (!init_wayland()) {
        std::cerr << "Failed to initialize Wayland" << std::endl;
        return false;
    }
    
    if (!init_egl()) {
        std::cerr << "Failed to initialize EGL" << std::endl;
        return false;
    }
    
    // Set window title
    if (xdg_toplevel) {
        xdg_toplevel_set_title(xdg_toplevel, title);
    }
    
    // Commit surface
    wl_surface_commit(surface);
    
    return true;
}

bool WaylandManager::init_wayland() {
    // Connect to Wayland display
    display = wl_display_connect(nullptr);
    if (!display) {
        std::cerr << "Failed to connect to Wayland display" << std::endl;
        return false;
    }
    
    // Get registry to enumerate global objects
    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, this);
    
    // Wait for globals to be advertised
    wl_display_roundtrip(display);
    
    if (!compositor) {
        std::cerr << "Compositor not available" << std::endl;
        return false;
    }
    
    if (!xdg_wm_base) {
        std::cerr << "XDG WM Base not available" << std::endl;
        return false;
    }
    
    // Create surface
    surface = wl_compositor_create_surface(compositor);
    if (!surface) {
        std::cerr << "Failed to create surface" << std::endl;
        return false;
    }
    
    // Create XDG surface
    xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, surface);
    xdg_surface_add_listener(xdg_surface, &xdg_surface_listener, this);
    
    // Create XDG toplevel (window)
    xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
    xdg_toplevel_add_listener(xdg_toplevel, &xdg_toplevel_listener, this);
    
    return true;
}

bool WaylandManager::init_egl() {
    // Get EGL display
    egl_display = eglGetDisplay((EGLNativeDisplayType)display);
    if (egl_display == EGL_NO_DISPLAY) {
        std::cerr << "Failed to get EGL display" << std::endl;
        return false;
    }
    
    // Initialize EGL
    if (!eglInitialize(egl_display, nullptr, nullptr)) {
        std::cerr << "Failed to initialize EGL" << std::endl;
        return false;
    }
    
    // Choose EGL config
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
    
    // Create EGL context
    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, context_attribs);
    if (egl_context == EGL_NO_CONTEXT) {
        std::cerr << "Failed to create EGL context" << std::endl;
        return false;
    }
    
    // Create EGL window
    egl_window = wl_egl_window_create(surface, width, height);
    if (!egl_window) {
        std::cerr << "Failed to create EGL window" << std::endl;
        return false;
    }
    
    // Create EGL surface
    egl_surface = eglCreateWindowSurface(egl_display, egl_config, 
                                        (EGLNativeWindowType)egl_window, nullptr);
    if (egl_surface == EGL_NO_SURFACE) {
        std::cerr << "Failed to create EGL surface" << std::endl;
        return false;
    }
    
    // Make context current
    if (!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context)) {
        std::cerr << "Failed to make EGL context current" << std::endl;
        return false;
    }
    
    return true;
}

void WaylandManager::process_events() {
    while (wl_display_prepare_read(display) != 0) {
        wl_display_dispatch_pending(display);
    }
    
    if (wl_display_flush(display) < 0) {
        wl_display_cancel_read(display);
        return;
    }
    
    wl_display_read_events(display);
    wl_display_dispatch_pending(display);
}

void WaylandManager::swap_buffers() {
    eglSwapBuffers(egl_display, egl_surface);
}

// Event handlers
void WaylandManager::registry_global(void* data, struct wl_registry* registry,
                                    uint32_t name, const char* interface, uint32_t version) {
    WaylandManager* wm = static_cast<WaylandManager*>(data);
    
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        wm->compositor = (struct wl_compositor*)wl_registry_bind(registry, name, 
                                                               &wl_compositor_interface, 4);
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        wm->xdg_wm_base = (struct xdg_wm_base*)wl_registry_bind(registry, name,
                                                               &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(wm->xdg_wm_base, &xdg_wm_base_listener, wm);
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        wm->seat = (struct wl_seat*)wl_registry_bind(registry, name, &wl_seat_interface, 1);
        // Setup pointer if available
        wm->pointer = wl_seat_get_pointer(wm->seat);
        if (wm->pointer) {
            wl_pointer_add_listener(wm->pointer, &pointer_listener, wm);
        }
    }
}

void WaylandManager::registry_global_remove(void* data, struct wl_registry* registry, uint32_t name) {
    // Handle global object removal if needed
}

void WaylandManager::xdg_surface_configure(void* data, struct xdg_surface* surface, uint32_t serial) {
    WaylandManager* wm = static_cast<WaylandManager*>(data);
    xdg_surface_ack_configure(surface, serial);
    wm->configured = true;
}

void WaylandManager::xdg_toplevel_configure(void* data, struct xdg_toplevel* toplevel,
                                           int32_t width, int32_t height, struct wl_array* states) {
    WaylandManager* wm = static_cast<WaylandManager*>(data);
    
    if (width > 0 && height > 0) {
        wm->width = width;
        wm->height = height;
        
        if (wm->egl_window) {
            wl_egl_window_resize(wm->egl_window, width, height, 0, 0);
        }
    }
}

void WaylandManager::xdg_toplevel_close(void* data, struct xdg_toplevel* toplevel) {
    WaylandManager* wm = static_cast<WaylandManager*>(data);
    wm->running = false;
}

void WaylandManager::xdg_wm_base_ping(void* data, struct xdg_wm_base* shell, uint32_t serial) {
    xdg_wm_base_pong(shell, serial);
}

// Pointer event handlers
void WaylandManager::pointer_enter(void* data, struct wl_pointer* pointer, uint32_t serial,
                                  struct wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy) {
    // Handle pointer enter
}

void WaylandManager::pointer_leave(void* data, struct wl_pointer* pointer, uint32_t serial,
                                  struct wl_surface* surface) {
    // Handle pointer leave
}

void WaylandManager::pointer_motion(void* data, struct wl_pointer* pointer, uint32_t time,
                                   wl_fixed_t sx, wl_fixed_t sy) {
    // Handle pointer motion
}

void WaylandManager::pointer_button(void* data, struct wl_pointer* pointer, uint32_t serial,
                                   uint32_t time, uint32_t button, uint32_t state) {
    // Handle pointer button events
    WaylandManager* wm = static_cast<WaylandManager*>(data);
    
    // Check if close button was clicked (implement later)
    // For now, any button click closes the window
    if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
        wm->running = false;
    }
}

void WaylandManager::pointer_axis(void* data, struct wl_pointer* pointer, uint32_t time,
                                 uint32_t axis, wl_fixed_t value) {
    // Handle scroll events
}

void WaylandManager::cleanup() {
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
    
    if (xdg_toplevel) {
        xdg_toplevel_destroy(xdg_toplevel);
    }
    
    if (xdg_surface) {
        xdg_surface_destroy(xdg_surface);
    }
    
    if (surface) {
        wl_surface_destroy(surface);
    }
    
    if (pointer) {
        wl_pointer_destroy(pointer);
    }
    
    if (seat) {
        wl_seat_destroy(seat);
    }
    
    if (xdg_wm_base) {
        xdg_wm_base_destroy(xdg_wm_base);
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