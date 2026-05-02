#ifndef __TEEOS__GUI__WINDOW_H
#define __TEEOS__GUI__WINDOW_H

    #include <gui/widget.h>
    #include <stdbool.h>

    typedef struct window
    {
        composite_widget_t base; 
    
        bool is_dragging;           
    } window_t;

    void window_init(window_t* window, widget_t* parent,
                     int32_t x, int32_t y, int32_t w, int32_t h,
                     uint8_t r, uint8_t g, uint8_t b);

    void window_on_mouse_down(window_t* window, int32_t x, int32_t y, uint8_t button);
    void window_on_mouse_up(window_t* window, int32_t x, int32_t y, uint8_t button);
    void window_on_mouse_move(window_t* window, int32_t oldx, int32_t oldy, int32_t newx, int32_t newy);

#endif