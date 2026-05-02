#ifndef __MYOS__GUI__DESKTOP_H
#define __MYOS__GUI__DESKTOP_H

#include <gui/widget.h>

/*
 * @brief Desktop widget structure - root container for all GUI elements
 */
typedef struct 
{
    composite_widget_t base;  // Inherit from composite widget
    int32_t mouse_x; 
    int32_t mouse_y;
} desktop_t;

// Desktop initialization
void desktop_init(desktop_t* desktop, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b);

#endif
