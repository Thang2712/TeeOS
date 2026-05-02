
#ifndef __TEEOS__GUI__WIDGET_H
#define __TEEOS__GUI__WIDGET_H
    #include <common/types.h>
    #include <common/graphicscontext.h>

    /*
     * @brief Base Widget structure
     */
    typedef struct widget
    {
        struct widget* parent;
        int32_t x;
        int32_t y;
        int32_t w;
        int32_t h;

        int32_t r;
        int32_t g;
        int32_t b;
        bool focusable;

        // Virtual function table
        void (*draw)(struct widget* self, graphics_context_t* gc);
        void (*get_focus)(struct widget* self, struct widget* widget);
        void (*model_to_screen)(struct widget* self, int32_t* x, int32_t* y);

        void (*on_mouse_down)(struct widget* self, int32_t x, int32_t y);
        void (*on_mouse_up)(struct widget* self, int32_t x, int32_t y);
        void (*on_mouse_move)(struct widget* self, int32_t oldx, int32_t oldy, int32_t newx, int32_t newy);

        void (*on_key_down)(struct widget* self, int32_t x, int32_t y);
        void (*on_key_up)(struct widget* self, int32_t x, int32_t y);
    } widget_t;

    /*
     * @brief Composite Widget structure
     * Simulates inheritance by embedding the base widget_t as the first memeber
     */
    typedef struct composite_widget
    {
        widget_t base;  // Inheritance simulation
        widget_t* children[100];
        int num_children;
        widget_t* focussed_child;
    } composite_widget_t;

    // Widget Base Function
    void widget_init(widget_t* widget, widget_t* parent, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b);

    // Default Implementation
    void widget_default_draw(widget_t* self, graphics_context_t* gc);
    void widget_model_to_screen(widget_t* self, int32_t* x, int32_t* y);

    // Composite Widget Functions
    void composite_widget_init(composite_widget_t* composite, widget_t* parent, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b);
    bool composite_widget_add_child(composite_widget_t* composite, widget_t* child);

    // Overridden Implementations
    void composite_widget_draw(widget_t* self, graphics_context_t* gc);
    void composite_widget_on_mouse_draw(widget_t* self, int32_t x, int32_t y);

#endif
