
#include <gui/widget.h>

// Forward declarations
void widget_default_get_focus(widget_t* self, widget_t* widget);
void widget_default_on_mouse_down(widget_t* self, int32_t x, int32_t y);
void composite_widget_get_focus(widget_t* self, widget_t* widget);
void composite_widget_on_mouse_down(widget_t* self, int32_t x, int32_t y);
void composite_widget_on_mouse_up(widget_t* self, int32_t x, int32_t y);
void composite_widget_on_mouse_move(widget_t* self, int32_t oldx, int32_t oldy, int32_t newx, int32_t newy);
void composite_widget_on_key_down(widget_t* self, int32_t x, int32_t y);
void composite_widget_on_key_up(widget_t* self, int32_t x, int32_t y);
void composite_widget_draw(widget_t* self, graphics_context_t* gc);

// @brief Initializes the base widget members and sets default virtual functions
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

    // Assign default virtual function implementations
    widget->draw = &widget_default_draw;
    widget->get_focus = &widget_default_get_focus;
    widget->model_to_screen = &widget_model_to_screen;
    widget->on_mouse_down = &widget_default_on_mouse_down;

    // Initialize other handlers to null by default if not used
    widget->on_mouse_up = 0;
    widget->on_mouse_move = 0;
    widget->on_key_down = 0;
    widget->on_key_up = 0;
}

void widget_default_get_focus(widget_t* self, widget_t* widget)
{
    if (self->parent != 0)
        self->parent->get_focus(self->parent, widget);
}

void widget_model_to_screen(widget_t* self, int32_t* x, int32_t* y)
{
    if (self->parent != 0)
        self->parent->model_to_screen(self, x, y);

    *x += self->x;
    *y += self->y;
}

void widget_default_draw(widget_t* self, graphics_context_t* gc)
{
    int32_t X = 0;
    int32_t Y = 0;

    self->model_to_screen(self, &X, &Y);
    vga_fill_rectangle(gc, X, Y, self->w, self->h, self->r, self->g, self->b);
}

void widget_default_on_mouse_down(widget_t* self, int32_t x, int32_t y)
{
    if (self->focusable)
        self->get_focus(self, self);
}

void composite_widget_init(composite_widget_t* composite, widget_t* parent, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b)
{
    widget_init((widget_t*)composite, parent, x, y, w, h, r, g, b);

    composite->focussed_child = 0;
    composite->num_children = 0;

    // Override Virtual functions
    composite->base.draw = &composite_widget_draw;
    composite->base.get_focus = &composite_widget_get_focus;
    composite->base.on_mouse_down = &composite_widget_on_mouse_down;
    composite->base.on_mouse_up = &composite_widget_on_mouse_up;
    composite->base.on_mouse_move = &composite_widget_on_mouse_move;
    composite->base.on_key_down = &composite_widget_on_key_down;
    composite->base.on_key_up = &composite_widget_on_key_up;
}

void composite_widget_get_focus(widget_t* self, widget_t* widget)
{
    composite_widget_t* composite = (composite_widget_t*)self;
    if (widget != 0 && widget->focusable)
        composite->focussed_child = widget;
    if (self->parent != 0)
        self->parent->get_focus(self->parent, self);
}

void composite_widget_draw(widget_t* self, graphics_context_t* gc)
{
    composite_widget_t* composite = (composite_widget_t*)self;

    // First, draw the background (base widget)
    widget_default_draw(self, gc);

    // Draw children from back to front
    for (int i = composite->num_children - 1; i >= 0; --i)
        composite->children[i]->draw(composite->children[i], gc);
}

/*
 * @brief Helper to check if a point is withn widget bounds
 * Note: Coordiantes passed should be relative to the widget's parent
 */
bool widget_contains_coordinates(widget_t* self, int32_t x, int32_t y)
{
    return (x >= self-> x && x < self-> x + self->w &&
            y >= self-> y && y < self->y + self->h );
}

void composite_widget_on_mouse_down(widget_t* self, int32_t x, int32_t y)
{
    composite_widget_t* composite = (composite_widget_t*)self;

    for (int i = 0; i < composite->num_children; ++i)
        if (widget_contains_coordinates(composite->children[i], x, y))
        {
            composite->children[i]->on_mouse_down(composite->children[i], x - composite->children[i]->x, y - composite->children[i]->y);
            break;
        }
}

// Repeat similar logic for MouseUp, MouseMove, etc.
void composite_widget_on_mouse_up(widget_t* self, int32_t x, int32_t y)
{
    composite_widget_t* composite = (composite_widget_t*)self;

    for (int i = 0; i < composite->num_children; ++i)
        if (widget_contains_coordinates(composite->children[i], x, y))
        {
            if (composite->children[i]->on_mouse_up)
                composite->children[i]->on_mouse_up(composite->children[i], x - composite->children[i]->x, y - composite->children[i]->y);
            break;
        }
}


void composite_widget_on_mouse_move(widget_t* self, int32_t oldx, int32_t oldy, int32_t newx, int32_t newy)
{
    composite_widget_t* composite = (composite_widget_t*) self;
    int first_child = -1 ;

    for (int i = 0; i < composite->num_children; ++i)
        if (widget_contains_coordinates(composite->children[i], oldx, oldy))
        {
            if (composite->children[i]->on_mouse_move)
                composite->children[i]->on_mouse_move(composite->children[i], oldx - composite->children[i]->x, oldy - composite->children[i]->y, newx - composite->children[i]->x, newy - composite->children[i]->y);
            first_child = i;
            break;
        }

    for (int i = 0; i < composite->num_children; ++i)
        if (widget_contains_coordinates(composite->children[i], newx, newy))
        {
            if (first_child != i && composite->children[i]->on_mouse_move)
                composite->children[i]->on_mouse_move(composite->children[i], oldx - composite->children[i]->x, oldy - composite->children[i]->y, newx - composite->children[i]->x, newy - composite->children[i]->y);
            break;
        }
}


void composite_widget_on_key_down(widget_t* self, int32_t x, int32_t y)
{
    composite_widget_t* composite = (composite_widget_t*)self;
    if (composite->focussed_child != 0 && composite->focussed_child->on_key_down)
        composite->focussed_child->on_key_down(composite->focussed_child, x, y);
}

void composite_widget_on_key_up(widget_t* self, int32_t x, int32_t y)
{
    composite_widget_t* composite = (composite_widget_t*) self;
    if (composite->focussed_child != 0 && composite->focussed_child->on_key_up)
        composite->focussed_child->on_key_up(composite->focussed_child, x, y);
}