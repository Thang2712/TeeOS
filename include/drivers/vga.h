
#ifndef __TEEOS__DRIVERS__VGA_H
#define __TEEOS__DRIVERS__VGA_H

    #include <common/types.h>
    #include <hardwarecommunication/port.h>
	#include <stdbool.h>

	/*
	 * @brief Structure representing the VGA (Video Graphics Array) driver.
	 * Contains I/O ports for various VGA controllers.
	 */
    typedef struct vga_driver
    {
    	// Miscellaneous Output Register
    	port8bit_t miscPort;

    	// CRT Controller Registers
    	port8bit_t crtcIndexPort;
    	port8bit_t crtcDataPort;

    	// Sequencer Registers
    	port8bit_t sequencerIndexPort;
    	port8bit_t sequencerDataPort;

    	// Graphics Controller Registers
    	port8bit_t graphicsControllerIndexPort;
    	port8bit_t graphicsControllerDataPort;

    	// Attribute Controller Registers
    	port8bit_t attributeControllerIndexPort;
    	port8bit_t attributeControllerReadPort;
    	port8bit_t attributeControllerWritePort;
    	port8bit_t attributeControllerResetPort;
    } vga_driver_t;

	// Initializarion and Cleanup
	void vga_init(vga_driver_t* vga);
	void vga_destroy(vga_driver_t* vga);

	// Internal Helper function

	// @brief Writes and array of 61 values to the VGA registers to set a video mode
	void vga_write_registers(vga_driver_t* vga, uint8_t* registers);

	/*
	 * @brief Retrieves the memory segment where the frame buffer is currently mapped.
	 * @return Pointer to the start of the video memory (e.g., 0xA0000).
	 */
	uint8_t* vga_get_frame_buffer_segment(vga_driver_t* vga);

	// @brief Coverts RGB values to closest VGA 8-bit color index.
	uint8_t vga_get_color_index(vga_driver_t* vga, uint8_t r, uint8_t g, uint8_t b);

	// Public API for Graphics Mode Manipulation

	// @brief Checks if the driver supports a specific resolution and color depth.
	bool vga_supports_mode(vga_driver_t* vga, uint32_t width, uint32_t height, uint32_t colordepth);

	// @brief Switches the VGA hardware to the specified mode
	bool vga_set_mode(vga_driver_t* vga, uint32_t width, uint32_t height, uint32_t colordepth);

	// Pixel Drawing Functions

	// @brief Draws a pixel using raw RGB values
	void vga_put_pixel_rgb(vga_driver_t* vga, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);

	// @brief Draws a pixel using a specific color index (0-255)
	void vga_put_pixel_index(vga_driver_t* vga, uint32_t x, uint32_t y, uint8_t colorIndex);

#endif
