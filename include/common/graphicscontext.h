
#ifndef __MYOS__COMMON__GRAPHICSCONTEXT__H
#define __MYOS__COMMON__GRAPHICSCONTEXT__H
    #include <drivers/vga.h>

    /*
     * @brief Provides a generic abstraction for display hardware.
     */
    typedef vga_driver_t graphics_context_t;

    /*
     * @brief Compatibility alias for Video Graphics Array
     */
    typedef graphics_context_t video_graphics_array_t;
#endif
