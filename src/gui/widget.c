#include <gui/widget.h>

// Forward declarations of internal implementation functions
void widget_default_draw(widget_t* self, graphics_context_t* gc);
void widget_default_get_focus(widget_t* self, widget_t* widget);
void widget_model_to_screen(widget_t* self, int32_t* x, int32_t* y);
void widget_default_on_mouse_down(widget_t* self, int32_t x, int32_t y, uint8_t button);

void composite_widget_draw(widget_t* self, graphics_context_t* gc);
void composite_widget_get_focus(widget_t* self, widget_t* widget);
void composite_widget_on_mouse_down(widget_t* self, int32_t x, int32_t y, uint8_t button);
void composite_widget_on_mouse_up(widget_t* self, int32_t x, int32_t y, uint8_t button);
void composite_widget_on_mouse_move(widget_t* self, int32_t oldx, int32_t oldy, int32_t newx, int32_t newy);
void composite_widget_on_key_down(widget_t* self, char str);
void composite_widget_on_key_up(widget_t* self, char str);

/**
 * @brief Base Widget Initialization
 */
void widget_init(widget_t *widget, widget_t* parent, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b)
{
    widget->parent = parent;
    widget->x = x;
    widget->y = y;
    widget->w = w;
    widget->h = h;
    widget->r = r;
    widget->g = g;
    widget->b = b;
    widget->focusable = true;

    // Assign default function pointers (Virtual Method Table simulation)
    widget->draw = &widget_default_draw;
    widget->get_focus = &widget_default_get_focus;
    widget->model_to_screen = &widget_model_to_screen;
    widget->on_mouse_down = &widget_default_on_mouse_down;

    // Initialize optional handlers to null
    widget->on_mouse_up = 0;
    widget->on_mouse_move = 0;
    widget->on_key_down = 0;
    widget->on_key_up = 0;
}

void widget_default_get_focus(widget_t* self, widget_t* widget)
{
    if (self->parent != 0 && self->parent->get_focus != 0)
        self->parent->get_focus(self->parent, widget);
}

void widget_model_to_screen(widget_t* self, int32_t* x, int32_t* y)
{
    if (self->parent != 0 && self->parent->model_to_screen != 0)
        self->parent->model_to_screen(self->parent, x, y);

    *x += self->x;
    *y += self->y;
}

void widget_default_draw(widget_t* self, graphics_context_t* gc)
{
    int32_t X = 0;
    int32_t Y = 0;
    self->model_to_screen(self, &X, &Y);
    vga_fill_rectangle((graphics_context_t*)gc, X, Y, self->w, self->h, self->r, self->g, self->b);
}

void widget_default_on_mouse_down(widget_t* self, int32_t x, int32_t y, uint8_t button)
{
    if (self->focusable && self->get_focus != 0)
        self->get_focus(self, self);
}

bool widget_contains_coordinates(widget_t* self, int32_t x, int32_t y)
{
    return (x >= self->x && x < self->x + self->w &&
            y >= self->y && y < self->y + self->h);
}

/**
 * @brief Composite Widget Initialization
 */
void composite_widget_init(composite_widget_t* composite, widget_t* parent, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b)
{
    widget_init((widget_t*)composite, parent, x, y, w, h, r, g, b);

    composite->focussed_child = 0;
    composite->num_children = 0;

    // Override virtual functions with composite-specific logic
    composite->base.draw = &composite_widget_draw;
    composite->base.get_focus = &composite_widget_get_focus;
    composite->base.on_mouse_down = &composite_widget_on_mouse_down;
    composite->base.on_mouse_up = &composite_widget_on_mouse_up;
    composite->base.on_mouse_move = &composite_widget_on_mouse_move;
    composite->base.on_key_down = &composite_widget_on_key_down;
    composite->base.on_key_up = &composite_widget_on_key_up;
}

bool composite_widget_add_child(composite_widget_t* composite, widget_t* child)
{
    if (composite->num_children >= 100)
        return false;
    composite->children[composite->num_children++] = child;
    return true;
}

void composite_widget_get_focus(widget_t* self, widget_t* widget)
{
    composite_widget_t* composite = (composite_widget_t*)self;
    if (widget != 0 && widget->focusable)
        composite->focussed_child = widget;
    
    if (self->parent != 0 && self->parent->get_focus != 0)
        self->parent->get_focus(self->parent, self);
}

void composite_widget_draw(widget_t* self, graphics_context_t* gc)
{
    composite_widget_t* composite = (composite_widget_t*)self;

    // 1. Draw background
    widget_default_draw(self, (graphics_context_t*)gc);

    // 2. Draw children
    for (int i = 0; i < composite->num_children; ++i)
    {
        if (composite->children[i]->draw != 0)
            composite->children[i]->draw(composite->children[i], (graphics_context_t*)gc);
    }
}

void composite_widget_on_mouse_down(widget_t* self, int32_t x, int32_t y, uint8_t button)
{
    composite_widget_t* composite = (composite_widget_t*)self;

    for (int i = 0; i < composite->num_children; ++i)
    {
        // Adjust coordinates to be relative to this composite widget's position
        if (widget_contains_coordinates(composite->children[i], x - self->x, y - self->y))
        {
            if (composite->children[i]->on_mouse_down != 0)
                composite->children[i]->on_mouse_down(composite->children[i], x - self->x, y - self->y, button);
            break;
        }
    }
}

void composite_widget_on_mouse_up(widget_t* self, int32_t x, int32_t y, uint8_t button)
{
    composite_widget_t* composite = (composite_widget_t*)self;

    for (int i = 0; i < composite->num_children; ++i)
    {
        if (widget_contains_coordinates(composite->children[i], x - self->x, y - self->y))
        {
            if (composite->children[i]->on_mouse_up != 0)
                composite->children[i]->on_mouse_up(composite->children[i], x - self->x, y - self->y, button);
            break;
        }
    }
}

void composite_widget_on_mouse_move(widget_t* self, int32_t oldx, int32_t oldy, int32_t newx, int32_t newy)
{
    composite_widget_t* composite = (composite_widget_t*)self;

    for (int i = 0; i < composite->num_children; ++i)
    {
        // For mouse move, we often check both old and new positions to handle hover/drag transitions
        if (widget_contains_coordinates(composite->children[i], oldx - self->x, oldy - self->y) ||
            widget_contains_coordinates(composite->children[i], newx - self->x, newy - self->y))
        {
            if (composite->children[i]->on_mouse_move != 0)
                composite->children[i]->on_mouse_move(composite->children[i], 
                    oldx - self->x, oldy - self->y, 
                    newx - self->x, newy - self->y);
        }
    }
}

void composite_widget_on_key_down(widget_t* self, char str)
{
    composite_widget_t* composite = (composite_widget_t*)self;
    if (composite->focussed_child != 0 && composite->focussed_child->on_key_down != 0)
        composite->focussed_child->on_key_down(composite->focussed_child, str);
}

void composite_widget_on_key_up(widget_t* self, char str)
{
    composite_widget_t* composite = (composite_widget_t*)self;
    if (composite->focussed_child != 0 && composite->focussed_child->on_key_up != 0)
        composite->focussed_child->on_key_up(composite->focussed_child, str);
}