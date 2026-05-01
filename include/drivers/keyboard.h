
#ifndef __TEEOS__DRIVERS__KEYBOARD_H
#define __TEEOS__DRIVERS__KEYBOARD_H

    #include <common/types.h>
    #include <hardwarecommunication/interrupts.h>
    #include <hardwarecommunication/port.h>

    /*
     * Structure representing the Keyboard Driver and its associated I/O ports.
     */
    struct KeyboardDriver
    {
        struct InterruptHandler base;
        struct Port8Bit dataport;       // Port 0x60: Data Register
        struct Port8Bit commandport;    // Port 0x64: Status/ Command Register
        struct KeyboardEvenHandler* handler; // User-defined event handlers for key events
    };

    struct KeyboardEvenHandler
    {
        void (*OnKeyDown)(char);
        void (*OnKeyUp)(char);
    };


    /*
     * Initializes the keyboard driver hardware and internal state.
     * @param driver Pointer to the KeyboardDriver instance.
     * @param manager Pointer to the Interrupt Manager to register the handler.
     */
    void init_keyboard_driver(struct KeyboardDriver *driver, struct InterruptManager *manager, struct KeyboardEvenHandler* handler);
    void keyboard_activate(struct KeyboardDriver* driver);

    /*
     * The interrupt service routine (ISR) for keyboard events (IRQ1)
     * This function is called by the Interrupt Manager when a key is pressed or released
     * @param esp The current stack pointer
     * @param The updated stack pointer after processing
     */
    uint32_t handle_keyboard_interrupt(struct KeyboardDriver *driver,uint32_t esp);

#endif

