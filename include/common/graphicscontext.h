
#ifndef __MYOS__COMMON__GRAPHICSCONTEXT__H
#define __MYOS__COMMON__GRAPHICSCONTEXT__H
    #include <drivers/vga.h>

    /*
     * @brief Provides a generic abstraction for display hardware.
     */
    typedef struct graphics_context 
    {
        vga_driver_t* vga; 

        int32_t width; 
        int32_t height;
    } graphics_context_t;

    /*
     * @brief Compatibility alias for Video Graphics Array
     */
    static inline void graphics_context_init(graphics_context_t* gc, vga_driver_t* vga)
    {
        gc->vga = vga; 
        gc->width = 320; 
        gc->height = 200;
    }

#endif
