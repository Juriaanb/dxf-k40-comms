#pragma once

#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "xdg-shell-client-protocol.h"

class WaylandManager {
private:
    // Wayland core objects
    struct wl_display* display;
    struct wl_registry* registry;
    struct wl_compositor* compositor;
    struct wl_surface* surface;
    struct wl_egl_window* egl_window;
    
    // XDG Shell (window management)
    struct xdg_wm_base* xdg_wm_base;
    struct xdg_surface* xdg_surface;
    struct xdg_toplevel* xdg_toplevel;
    
    // Input devices
    struct wl_seat* seat;
    struct wl_pointer* pointer;
    struct wl_keyboard* keyboard;
    
    // EGL context
    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
    EGLSurface egl_surface;
    
    // Window state
    int width, height;
    bool configured;
    bool running;
    
    bool init_wayland();
    bool init_egl();
    void cleanup();

public:
    // Event handlers (must be public for C callback access)
    static void registry_global(void* data, struct wl_registry* registry,
                               uint32_t name, const char* interface, uint32_t version);
    static void registry_global_remove(void* data, struct wl_registry* registry, uint32_t name);
    
    static void xdg_surface_configure(void* data, struct xdg_surface* surface, uint32_t serial);
    static void xdg_toplevel_configure(void* data, struct xdg_toplevel* toplevel,
                                      int32_t width, int32_t height, struct wl_array* states);
    static void xdg_toplevel_close(void* data, struct xdg_toplevel* toplevel);
    static void xdg_wm_base_ping(void* data, struct xdg_wm_base* shell, uint32_t serial);
    
    static void pointer_enter(void* data, struct wl_pointer* pointer, uint32_t serial,
                             struct wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy);
    static void pointer_leave(void* data, struct wl_pointer* pointer, uint32_t serial,
                             struct wl_surface* surface);
    static void pointer_motion(void* data, struct wl_pointer* pointer, uint32_t time,
                              wl_fixed_t sx, wl_fixed_t sy);
    static void pointer_button(void* data, struct wl_pointer* pointer, uint32_t serial,
                              uint32_t time, uint32_t button, uint32_t state);
    static void pointer_axis(void* data, struct wl_pointer* pointer, uint32_t time,
                            uint32_t axis, wl_fixed_t value);

public:
    WaylandManager();
    ~WaylandManager();
    
    bool initialize(int width, int height, const char* title);
    void process_events();
    void swap_buffers();
    bool should_close() const { return !running; }
    
    int get_width() const { return width; }
    int get_height() const { return height; }
    bool is_configured() const { return configured; }
};