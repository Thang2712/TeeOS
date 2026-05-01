#include <drivers/vga.h>


/*
 * @brief Initializes the VGA driver by mapping internal I/O ports
 * Sets up the 8-bit ports for the Misceellaneous, CRTC, Sequencer,
 * Graphics Controller, and Attribute Controller registers.
 */
void vga_init(vga_driver_t* vga)
{
    init_port8bit(&vga->miscPort, 0x3C2);
    init_port8bit(&vga->crtcIndexPort, 0x3D4);
    init_port8bit(&vga->crtcDataPort, 0x3D5);
    init_port8bit(&vga->sequencerIndexPort, 0x3C4);
    init_port8bit(&vga->sequencerDataPort, 0x3C5);
    init_port8bit(&vga->graphicsControllerIndexPort, 0x3CE);
    init_port8bit(&vga->graphicsControllerDataPort, 0x3CF);
    init_port8bit(&vga->attributeControllerIndexPort, 0x3C0);
    init_port8bit(&vga->attributeControllerReadPort, 0x3C1);
    init_port8bit(&vga->attributeControllerWritePort, 0x3C0);
    init_port8bit(&vga->attributeControllerResetPort, 0x3DA);
}

/*
 * @brief Massive register write to configure VGA hardware state.
 * This follows the standard VGA sequence: Misc->Sequencer->CRTC->GC->Ac.
 */
void vga_write_registers(vga_driver_t* vga, uint8_t* registers)
{
    // Write Miscellaneous Output Register
    vga->miscPort.Write(&vga->miscPort, *(registers++));

    // Write Sequencer Register (5 registers)
    for(uint8_t i = 0; i < 5; i++)
    {
        vga->sequencerIndexPort.Write(&vga->sequencerIndexPort, i);
        vga->sequencerDataPort.Write(&vga->sequencerDataPort, *(registers++));
    }

    // Unlock CRTC registers: bit 7 of Index of 0x11 must be 0 to write to registers 0-7
    vga->crtcIndexPort.Write(&vga->crtcIndexPort, 0x03);
    vga->crtcDataPort.Write(&vga->crtcDataPort, vga->crtcDataPort.Read(&vga->crtcDataPort) | 0x80);
    vga->crtcIndexPort.Write(&vga->crtcIndexPort, 0x11);
    vga->crtcDataPort.Write(&vga->crtcDataPort, vga->crtcDataPort.Read(&vga->crtcDataPort) & ~0x80);

    // Ensure the data array reflects the unlock state for the loop below
    registers[0x03] |= 0x80;
    registers[0x11] &= ~0x80;

    // Write CRTC Registers (25 Registers)
    for(uint8_t i = 0; i < 25; i++)
    {
        vga->crtcIndexPort.Write(&vga->crtcIndexPort, i);
        vga->crtcDataPort.Write(&vga->crtcDataPort, *(registers++));
    }

    // Write Graphics Controller Registers (9 Registers)
    for(uint8_t i = 0; i < 9; i++)
    {
        vga->graphicsControllerIndexPort.Write(&vga->graphicsControllerIndexPort, i);
        vga->graphicsControllerDataPort.Write(&vga->graphicsControllerDataPort, *(registers++));
    }

    // Write Attribute Controller Registers (21 Registers)
    for(uint8_t i = 0; i < 21; i++)
    {
        vga->attributeControllerResetPort.Read(&vga->attributeControllerResetPort);
        vga->attributeControllerIndexPort.Write(&vga->attributeControllerIndexPort, i);
        vga->attributeControllerWritePort.Write(&vga->attributeControllerWritePort, *(registers++));
    }

    // Re-enable video signal by writing to the PAS bit (bit 5)
    vga->attributeControllerResetPort.Read(&vga->attributeControllerResetPort);
    vga->attributeControllerIndexPort.Write(&vga->attributeControllerIndexPort, 0x20);
}

/*
 * @brief Validation for requested video modes
 * Currently only supports standard Mode 13h (320x200 256-colors)
 */
bool vga_supports_mode(vga_driver_t* vga, uint32_t width, uint32_t height, uint32_t colordepth)
{
    return width == 320 && height == 200 && colordepth == 8;
}

//  @brief Sets the VGA hardware to the specified mode using pre-defined register values
bool vga_set_mode(vga_driver_t* vga, uint32_t width, uint32_t height, uint32_t colordepth)
{
    if(!vga_supports_mode(vga, width, height, colordepth))
        return false;

    // Register values for standard VGA Mode 13h (320x200x8bpp)
    uint8_t g_320x200x256[] =
    {
        // MISC
        0x63,
        // SEQ
        0x03, 0x01, 0x0F, 0x00, 0x0E,
        // CRTC
        0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
        0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
        0xFF,
        // GC
        0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
        0xFF,
        // AC
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x41, 0x00, 0x0F, 0x00, 0x00
    };

    vga_write_registers(vga, g_320x200x256);
    return true;
}

/*
 * @brief Determines the segment of memory where the VGA frame buffer resides.
 * Reads the Graphics Controller register 0x06 to check the memory map bits.
 */
uint8_t* vga_get_frame_buffer_segment(vga_driver_t* vga)
{
    vga->graphicsControllerIndexPort.Write(&vga->graphicsControllerIndexPort, 0x06);
    uint8_t segmentNumber = vga->graphicsControllerDataPort.Read(&vga->graphicsControllerDataPort) & (3 << 2);

    switch(segmentNumber)
    {
        case 0 << 2:
            return (uint8_t*)0x00000;
        case 1 << 2:
            return (uint8_t*)0xA0000;   // Standard for graphics modes
        case 2 << 2:
            return (uint8_t*)0xB0000;
        case 3 << 2:
            return (uint8_t*)0xB8000;   // Standard for text modes
        default:
            return (uint8_t*)0xA0000;
    }
}

// @brief Plots a pixel on the screen using a palette index
void vga_put_pixel_index(vga_driver_t* vga, uint32_t x, uint32_t y, uint8_t colorIndex)
{
    if(x >= 320 || y >= 200)
        return;
    // Calculation for Mode 13h: segment + (width * y) + x
    uint8_t* pixelAddress = vga_get_frame_buffer_segment(vga) + 320 * y + x;
    *pixelAddress = colorIndex;
}

// @brief Simple RGB-to-Index lookup for basic VGA colors
uint8_t vga_get_color_index(vga_driver_t* vga, uint8_t r, uint8_t g, uint8_t b)
{
    // Example: Dark blue mapping
    if(r == 0x00 && g == 0x00 && b == 0xA8)
        return 0x01;
    return 0x00;
}

// @brief Plots a pixel on the screen using RGB values
void vga_put_pixel_rgb(vga_driver_t* vga, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    vga_put_pixel_index(vga, x, y, vga_get_color_index(vga, r, g, b));
}