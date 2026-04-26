
#ifndef __MOUSE_H
#define __MOUSE_H

    #include "types.h"
    #include "port.h"
    #include "interrupts.h"

    /*
     * Structure representing the MouseDriver
     * Stores all the state of mouse, including coordinates and button states.
     */
    struct MouseDriver
    {
        struct Port8Bit dataport;       // Port 0x60
        struct Port8Bit commandport;    // Port 0x64

        uint8_t buffer[3];              // Buffer to store 3-byte mouse packets
        uint8_t offset;                 // Current byte offset in buffer (0-2)
        uint8_t buttons;                // Bitmask for mouse buttons (Left, Right, Middle)

        int8_t x, y;                    // Current relative movement
    };

    /*
     * Initializes the PS/2 mouse hardware.
     * @param driver: Pointer to the MouseDriver struct
     * @param manager: Pointer to the InterruptManager for IRQ12 registration.
     */
    void init_mouse_driver(struct MouseDriver *driver, struct InterruptManager *manager);

    /*
     * Processes mouse interrupt (IRQ12)
     * Handles the 3-byte packet protocol to update movement and clicks
     * @param driver: Pointer to the MouseDriver struct
     * @param esp: The current stack pointer.
     */
    uint32_t handle_mouse_interrupt(struct MouseDriver *driver, uint32_t esp);
#endif
