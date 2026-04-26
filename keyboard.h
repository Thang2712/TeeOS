
#ifndef __KEYBOARD_H
#define __KEYBOARD_H

    #include "types.h"
    #include "interrupts.h"
    #include "port.h"

    /*
     * Structure representing the Keyboard Driver and its associated I/O ports.
     */
    struct KeyboardDriver
    {
        struct Port8Bit dataport;       // Port 0x60: Data Register
        struct Port8Bit commandport;    // Port 0x64: Status/ Command Register
    };

    /*
     * Initializes the keyboard driver hardware and internal state.
     * @param driver Pointer to the KeyboardDriver instance.
     * @param manager Pointer to the Interrupt Manager to register the handler.
     */
    void init_keyboard_driver(struct KeyboardDriver *driver, struct InterruptManager *manager);

    /*
     * The interrupt service routine (ISR) for keyboard events (IRQ1)
     * This function is called by the Interrupt Manager when a key is pressed or released
     * @param esp The current stack pointer
     * @param The updated stack pointer after processing
     */
    uint32_t handle_keyboard_interrupt(struct KeyboardDriver *driver,uint32_t esp);

#endif

