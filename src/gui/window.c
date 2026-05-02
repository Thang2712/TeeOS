#include <gui/window.h>

/**
 * @brief Initializes a Window widget.
 * Windows can be dragged by holding the left mouse button (button 1).
 */
void window_init(window_t* window, widget_t* parent,
                 int32_t x, int32_t y, int32_t w, int32_t h,
                 uint8_t r, uint8_t g, uint8_t b)
{
    // Initialize the base CompositeWidget
    composite_widget_init((composite_widget_t*)window, parent, x, y, w, h, r, g, b);
    
    // Set initial dragging state
    window->is_dragging = false;
}

/**
 * @brief Handles mouse button press.
 * Activates dragging mode if the left mouse button is pressed.
 */
void window_on_mouse_down(window_t* window, int32_t x, int32_t y, uint8_t button)
{
    // If left mouse button (1) is pressed, start dragging
    window->is_dragging = (button == 1);
    
    // Pass event down to children via the composite widget logic
    composite_widget_on_mouse_down((widget_t*)window, x, y, button);
}

/**
 * @brief Handles mouse button release.
 * Terminates the dragging state.
 */
void window_on_mouse_up(window_t* window, int32_t x, int32_t y, uint8_t button)
{
    window->is_dragging = false;
    
    // Pass event down to children
    composite_widget_on_mouse_up((widget_t*)window, x, y, button);
}

/**
 * @brief Handles mouse movement and updates window position if dragging.
 */
void window_on_mouse_move(window_t* window, int32_t oldx, int32_t oldy, 
                          int32_t newx, int32_t newy)
{
    if (window->is_dragging)
    {
        // Update the window's position based on the relative movement of the mouse
        // Note: we cast to widget_t to access the 'x' and 'y' members of the base struct
        ((widget_t*)window)->x += (newx - oldx);
        ((widget_t*)window)->y += (newy - oldy);
    }
    
    // Pass event down to children
    composite_widget_on_mouse_move((widget_t*)window, oldx, oldy, newx, newy);
}