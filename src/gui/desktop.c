#include <gui/desktop.h>

void desktop_init(desktop_t* desktop, int32_t w, int32_t h, 
                  uint8_t r, uint8_t g, uint8_t b)
{
    composite_widget_init((composite_widget_t*)desktop, 0, 0, 0, w, h, r, g, b);
    
    desktop->mouse_x = w / 2;
    desktop->mouse_y = h / 2;
}

void desktop_draw(widget_t* self, graphics_context_t* gc)
{
    desktop_t* d = (desktop_t*)self;

    composite_widget_draw(self, gc);

    vga_fill_rectangle(gc, d->mouse_x - 2, d->mouse_y, 5, 1, 0xFF, 0xFF, 0xFF); // Ngang
    vga_fill_rectangle(gc, d->mouse_x, d->mouse_y - 2, 1, 5, 0xFF, 0xFF, 0xFF); // Dọc
}