#include <stdio.h>

#include <foundation/foundation.h>

ft_view_t *image_view = NULL;

#define WINDOW_SHADOW_THICKNESS     40
#define WINDOW_RESIZE_THICKNESS     5
#define WINDOW_BORDER_THICKNESS     1
#define WINDOW_TITLE_BAR_HEIGHT     30
#define WINDOW_DEFAULT_WIDTH        200
#define WINDOW_DEFAULT_HEIGHT       200

struct window {
    ft_desktop_surface_t *desktop_surface;
    ft_view_t *shadow;
    ft_view_t *resize;
    ft_view_t *border;
    ft_view_t *title_bar;
    ft_view_t *body;
};

struct window *window_global = NULL;

ft_rect_t get_shadow_geometry(struct window *window)
{
    ft_surface_t *surface = ft_desktop_surface_surface(window->desktop_surface);
    const ft_size_t *window_size = ft_surface_size(surface);

    ft_rect_t geometry;
    geometry.pos.x = 0;
    geometry.pos.y = 0;
    geometry.size.width = window_size->width; // + WINDOW_SHADOW_THICKNESS;
    geometry.size.height = window_size->height; // + WINDOW_SHADOW_THICKNESS;

    return geometry;
}

ft_rect_t get_resize_geometry(struct window *window)
{
    ft_surface_t *surface = ft_desktop_surface_surface(window->desktop_surface);
    const ft_size_t *window_size = ft_surface_size(surface);

    ft_rect_t geometry;
    geometry.pos.x = WINDOW_SHADOW_THICKNESS;
    geometry.pos.y = WINDOW_SHADOW_THICKNESS;
    geometry.size.width = window_size->width
        - (WINDOW_SHADOW_THICKNESS * 2);
    geometry.size.height = window_size->height
        - (WINDOW_SHADOW_THICKNESS * 2);

    return geometry;
}

ft_rect_t get_border_geometry(struct window *window)
{
    ft_surface_t *surface = ft_desktop_surface_surface(window->desktop_surface);
    const ft_size_t *window_size = ft_surface_size(surface);

    ft_rect_t geometry;
    geometry.pos.x = WINDOW_RESIZE_THICKNESS;
    geometry.pos.y = WINDOW_RESIZE_THICKNESS;
    geometry.size.width = window_size->width
        - (WINDOW_SHADOW_THICKNESS * 2) - (WINDOW_RESIZE_THICKNESS * 2);
    geometry.size.height = window_size->height
        - (WINDOW_SHADOW_THICKNESS * 2) - (WINDOW_RESIZE_THICKNESS * 2);

    return geometry;
}

ft_rect_t get_title_bar_geometry(struct window *window)
{
    const ft_size_t border_size = ft_view_geometry(window->border)->size;

    ft_rect_t geometry;
    geometry.pos.x = WINDOW_BORDER_THICKNESS;
    geometry.pos.y = WINDOW_BORDER_THICKNESS;
    geometry.size.width = border_size.width - (WINDOW_BORDER_THICKNESS * 2);
    geometry.size.height = WINDOW_TITLE_BAR_HEIGHT;

    return geometry;
}

ft_rect_t get_body_geometry(struct window *window)
{
    const ft_size_t border_size = ft_view_geometry(window->border)->size;

    ft_rect_t geometry;
    geometry.pos.x = WINDOW_BORDER_THICKNESS;
    geometry.pos.y = WINDOW_BORDER_THICKNESS + WINDOW_TITLE_BAR_HEIGHT;
    geometry.size.width = border_size.width - (WINDOW_BORDER_THICKNESS * 2);
    geometry.size.height = border_size.height - (WINDOW_BORDER_THICKNESS * 2)
        - WINDOW_TITLE_BAR_HEIGHT;

    return geometry;
}


static void on_surface_resize(ft_event_t *event)
{
    fprintf(stderr, "on_resize\n");

    // Update shadow size.
    ft_rect_t shadow_geometry = get_shadow_geometry(window_global);
    ft_view_set_geometry(window_global->shadow, &shadow_geometry);

    // Update resize size.
    ft_rect_t resize_geometry = get_resize_geometry(window_global);
    ft_view_set_geometry(window_global->resize, &resize_geometry);

    // Update border size.
    ft_rect_t border_geometry = get_border_geometry(window_global);
    ft_view_set_geometry(window_global->border, &border_geometry);

    // Update title bar size.
    ft_rect_t title_bar_geometry = get_title_bar_geometry(window_global);
    ft_view_set_geometry(window_global->title_bar, &title_bar_geometry);

    // Update body size.
    ft_rect_t body_geometry = get_body_geometry(window_global);
    ft_view_set_geometry(window_global->body, &body_geometry);
}

static void on_title_bar_press(ft_event_t *event)
{
    ft_desktop_surface_toplevel_move(window_global->desktop_surface);
}


static void init_window(struct window *window)
{
    ft_color_t color;
    ft_size_t size;
    ft_rect_t geometry;
    ft_surface_t *surface = ft_desktop_surface_surface(window->desktop_surface);

    // Set the total surface size.
    size.width = WINDOW_DEFAULT_WIDTH + (WINDOW_SHADOW_THICKNESS * 2);
    size.height = WINDOW_DEFAULT_HEIGHT + (WINDOW_SHADOW_THICKNESS * 2);
    ft_surface_set_size(surface, &size);

    // Transparent root view.
    color.r = 0;
    color.g = 0;
    color.b = 0;
    color.a = 0;
    ft_view_set_color(ft_surface_root_view(surface), &color);

    // Set shadow.
    ft_rect_t shadow_geometry = get_shadow_geometry(window);
    window->shadow = ft_view_new(ft_surface_root_view(surface),
        &shadow_geometry);
    color.r = 100;
    color.g = 100;
    color.b = 100;
    color.a = 100;
    ft_view_set_color(window->shadow, &color);

    // Set resize.
    ft_rect_t resize_geometry = get_resize_geometry(window);
    window->resize = ft_view_new(window->shadow, &resize_geometry);

    // Set border.
    ft_rect_t border_geometry = get_border_geometry(window);
    window->border = ft_view_new(window->resize, &border_geometry);
    color.r = 100;
    color.g = 100;
    color.b = 100;
    color.a = 255;
    ft_view_set_color(window->border, &color);

    // Set title bar.
    ft_rect_t title_bar_geometry = get_title_bar_geometry(window);
    window->title_bar = ft_view_new(window->border, &title_bar_geometry);
    color.r = 180;
    color.g = 180;
    color.b = 180;
    color.a = 255;
    ft_view_set_color(window->title_bar, &color);

    ft_rect_t button_geometry;
    button_geometry.pos.x = 3;
    button_geometry.pos.y = 3;
    button_geometry.size.width = 24;
    button_geometry.size.height = 24;
    ft_view_t *close_button = ft_view_new(window->title_bar, &button_geometry);
    color.r = 255;
    color.g = 0;
    color.b = 0;
    ft_view_set_color(close_button, &color);

    button_geometry.pos.x = button_geometry.pos.x + 24 + 3;
    ft_view_t *minimize_button = ft_view_new(window->title_bar,
        &button_geometry);
    color.r = 255;
    color.g = 255;
    color.b = 0;
    ft_view_set_color(minimize_button, &color);

    button_geometry.pos.x = button_geometry.pos.x + 24 + 3;
    ft_view_t *maximize_button = ft_view_new(window->title_bar,
        &button_geometry);
    color.r = 0;
    color.g = 255;
    color.b = 0;
    ft_view_set_color(maximize_button, &color);

    // Set body.
    ft_rect_t body_geometry = get_body_geometry(window);
    window->body = ft_view_new(window->border, &body_geometry);
    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 255;
    ft_view_set_color(window->body, &color);
}

int main(int argc, char *argv[])
{
    ft_application_t *app = ft_application_new(argc, argv);

    struct window window;
    window.desktop_surface = ft_desktop_surface_new(
        FT_DESKTOP_SURFACE_ROLE_TOPLEVEL);

    init_window(&window);

    window_global = &window;

    // Resize event handler.
    ft_surface_add_event_listener(
        ft_desktop_surface_surface(window.desktop_surface),
        FT_EVENT_TYPE_RESIZE,
        on_surface_resize);

    // Title bar press event handler.
    ft_view_add_event_listener(window.title_bar,
        FT_EVENT_TYPE_POINTER_PRESS,
        on_title_bar_press);

    ft_desktop_surface_show(window.desktop_surface);

    return ft_application_exec(app);
}
