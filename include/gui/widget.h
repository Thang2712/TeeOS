#ifndef __TEEOS__GUI__WIDGET_H
#define __TEEOS__GUI__WIDGET_H

    #include <common/types.h>
    #include <common/graphicscontext.h>
    #include <stdbool.h>

    // Forward declaration
    struct widget;
    typedef struct widget widget_t;

    /*
     * @brief Base Widget structure
     */
    struct widget
    {
        widget_t* parent;
        int32_t x;
        int32_t y;
        int32_t w;
        int32_t h;

        uint8_t r; 
        uint8_t g;
        uint8_t b;
        bool focusable;

        // Virtual function table (Function Pointers)
        void (*draw)(widget_t* self, graphics_context_t* gc);
        void (*get_focus)(widget_t* self, widget_t* widget);
        void (*model_to_screen)(widget_t* self, int32_t* x, int32_t* y);
        bool (*contains_coordinates)(widget_t* self, int32_t x, int32_t y);

        void (*on_mouse_down)(widget_t* self, int32_t x, int32_t y, uint8_t button);
        void (*on_mouse_up)(widget_t* self, int32_t x, int32_t y, uint8_t button);
        void (*on_mouse_move)(widget_t* self, int32_t oldx, int32_t oldy, int32_t newx, int32_t newy);

        void (*on_key_down)(widget_t* self, char c);
        void (*on_key_up)(widget_t* self, char c);
    };

    /*
     * @brief Composite Widget structure
     */
    typedef struct composite_widget
    {
        widget_t base;
        widget_t* children[100];
        int32_t num_children;
        widget_t* focussed_child;
    } composite_widget_t;

    // --- Function Prototypes ---

    // Base Widget
    void widget_init(widget_t* widget, widget_t* parent, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b);
    void widget_default_draw(widget_t* self, graphics_context_t* gc);
    void widget_model_to_screen(widget_t* self, int32_t* x, int32_t* y);
    bool widget_default_contains_coordinates(widget_t* self, int32_t x, int32_t y);

    // Composite Widget 
    void composite_widget_init(composite_widget_t* composite, widget_t* parent, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b);
    bool composite_widget_add_child(composite_widget_t* composite, widget_t* child);
    
    // Overridden cho Composite
    void composite_widget_draw(widget_t* self, graphics_context_t* gc);
    void composite_widget_on_mouse_down(widget_t* self, int32_t x, int32_t y, uint8_t button);
    void composite_widget_on_mouse_up(widget_t* self, int32_t x, int32_t y, uint8_t button);
    void composite_widget_on_mouse_move(widget_t* self, int32_t oldx, int32_t oldy, int32_t newx, int32_t newy);
    void composite_widget_on_key_down(widget_t* self, char str);
    void composite_widget_on_key_up(widget_t* self, char str);

#endif