#ifndef __TEEOS__DRIVERS__VGA_H
#define __TEEOS__DRIVERS__VGA_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <stdbool.h>

// Forward declaration 
struct graphics_context;
typedef struct graphics_context graphics_context_t;

typedef struct vga_driver
{
    port8bit_t miscPort;
    port8bit_t crtcIndexPort;
    port8bit_t crtcDataPort;
    port8bit_t sequencerIndexPort;
    port8bit_t sequencerDataPort;
    port8bit_t graphicsControllerIndexPort;
    port8bit_t graphicsControllerDataPort;
    port8bit_t attributeControllerIndexPort;
    port8bit_t attributeControllerReadPort;
    port8bit_t attributeControllerWritePort;
    port8bit_t attributeControllerResetPort;
} vga_driver_t;

// Initialization
void vga_init(vga_driver_t* vga);

// Mode manipulation
bool vga_supports_mode(vga_driver_t* vga, uint32_t width, uint32_t height, uint32_t colordepth);
bool vga_set_mode(vga_driver_t* vga, uint32_t width, uint32_t height, uint32_t colordepth);

// Helper functions
uint8_t* vga_get_frame_buffer_segment(graphics_context_t* vga);
uint8_t vga_get_color_index(graphics_context_t* vga, uint8_t r, uint8_t g, uint8_t b);

// Pixel Drawing (Sử dụng graphics_context_t để tương thích với GUI)
void vga_put_pixel_index(graphics_context_t* vga, int32_t x, int32_t y, uint8_t colorIndex);
void vga_put_pixel_rgb(graphics_context_t* vga, int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b);
void vga_fill_rectangle(graphics_context_t* vga, int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b);

#endif