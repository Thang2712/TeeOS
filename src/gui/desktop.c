#include <gui/desktop.h>

void desktop_init(desktop_t* desktop, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b)
{
    composite_widget_init((composite_widget_t*)desktop, 0, 0, 0, w, h, r, g, b);
    desktop->mouse_x = w / 2; // Fixed naming
    desktop->mouse_y = h / 2;
}

void desktop_draw(widget_t* widget, graphics_context_t* gc)
{
    desktop_t* desktop = (desktop_t*)widget;
    // Draw background and windows via base class
    composite_widget_draw(&desktop->base.base, gc);
    
    // Draw mouse cursor
    for(int32_t i = 0; i < 4; i++)
    {
        vga_put_pixel_rgb(gc, desktop->mouse_x - i, desktop->mouse_y, 0xFF, 0xFF, 0xFF);
        vga_put_pixel_rgb(gc, desktop->mouse_x + i, desktop->mouse_y, 0xFF, 0xFF, 0xFF);
        vga_put_pixel_rgb(gc, desktop->mouse_x, desktop->mouse_y - i, 0xFF, 0xFF, 0xFF);
        vga_put_pixel_rgb(gc, desktop->mouse_x, desktop->mouse_y + i, 0xFF, 0xFF, 0xFF);
    }
}

void desktop_on_mouse_down(desktop_t* desktop, uint8_t button)
{
    composite_widget_on_mouse_down(&desktop->base.base, 
                                   desktop->mouse_x, desktop->mouse_y, button);
}

void desktop_on_mouse_up(desktop_t* desktop, uint8_t button)
{
    composite_widget_on_mouse_up(&desktop->base.base, 
                                 desktop->mouse_x, desktop->mouse_y, button);
}

void desktop_on_mouse_move(desktop_t* desktop, int32_t x, int32_t y)
{
    x /= 4;
    y /= 4;
    
    int32_t new_mouse_x = desktop->mouse_x + x;
    int32_t new_mouse_y = desktop->mouse_y + y;
    
    // Truy cập w, h thông qua struct cha
    int32_t screen_w = desktop->base.base.w;
    int32_t screen_h = desktop->base.base.h;

    if(new_mouse_x < 0) new_mouse_x = 0;
    if(new_mouse_x >= screen_w) new_mouse_x = screen_w - 1;
    
    if(new_mouse_y < 0) new_mouse_y = 0;
    if(new_mouse_y >= screen_h) new_mouse_y = screen_h - 1;
    
    composite_widget_on_mouse_move(&desktop->base.base, 
                                   desktop->mouse_x, desktop->mouse_y, 
                                   new_mouse_x, new_mouse_y);
    
    desktop->mouse_x = new_mouse_x;
    desktop->mouse_y = new_mouse_y;
}