
#ifndef __TEEOS__DRIVERS__MOUSE_H
#define __TEEOS__DRIVERS__MOUSE_H

    #include <common/types.h>
    #include <hardwarecommunication/port.h>
    #include <hardwarecommunication/interrupts.h>

    struct MouseEventHandler
    {
        void (*OnActivate) ();
        void (*OnMouseDown) (uint8_t button);
        void (*OnMouseUp) (uint8_t button);
        void (*OnMouseMove) (int8_t x, int8_t y);
    };
    /*
     * Structure representing the MouseDriver
     * Stores all the state of mouse, including coordinates and button states.
     */
    struct MouseDriver
    {
        struct InterruptHandler base;       // Base interrupt handler for registration with InterruptManager

        struct Port8Bit dataport;       // Port 0x60
        struct Port8Bit commandport;    // Port 0x64

        uint8_t buffer[3];              // Buffer to store 3-byte mouse packets
        uint8_t offset;                 // Current byte offset in buffer (0-2)
        uint8_t buttons;                // Bitmask for mouse buttons (Left, Right, Middle)

        int8_t x, y;                    // Current relative movement
        uint16_t click_feedback_counter; // Counter for click visual feedback (1-2 seconds)
        struct MouseEventHandler* handler; // User-defined event handlers for mouse events
    };

    /*
     * Initializes the PS/2 mouse hardware.
     * @param driver: Pointer to the MouseDriver struct
     * @param manager: Pointer to the InterruptManager for IRQ12 registration.
     */
    void init_mouse_driver(struct MouseDriver *driver, struct InterruptManager *manager, struct MouseEventHandler* handler);

    void mouse_activate(struct MouseDriver *driver);

    /*
     * Processes mouse interrupt (IRQ12)
     * Handles the 3-byte packet protocol to update movement and clicks
     * @param driver: Pointer to the MouseDriver struct
     * @param esp: The current stack pointer.
     */
    uint32_t handle_mouse_interrupt(struct MouseDriver *driver, uint32_t esp);

#endif
