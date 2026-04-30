#include "keyboard.h"
#include "driver.h"



void kprintf(char *str);

/*
 * Initializes the Keyboard Driver.
 * @param driver: Pointer to the KeyboardDriver struct instance.
 * @param manager: Pointer to the InterruptManager (for remapping IRQ1)
 */
void init_keyboard_driver(struct KeyboardDriver* driver, struct InterruptManager* manager, struct KeyboardEvenHandler* handler)
{
    driver->handler = handler;
    // Initialize the ports (using the init functions from my port.c)
    init_port8bit(&(driver->dataport), 0x60);
    init_port8bit(&(driver->commandport), 0x64);

    // 1. Drain the PS/2 controller buffer (with timeout to prevent infinite loop)
    int drain_timeout = 100000;
    while ((driver->commandport.Read(&(driver->commandport)) & 0x1) && drain_timeout-- > 0)
        driver->dataport.Read(&(driver->dataport));

    // 2. Activate the PS/2 keyboard interface (Command 0xae = enable)
    driver->commandport.Write(&(driver->commandport), 0xae);
    for(volatile int i = 0; i < 100000; i++);  // Longer delay

    // 3. Update Controller Configuration Byte
    driver->commandport.Write(&(driver->commandport), 0x20);    // Request config byte
    for(volatile int i = 0; i < 100000; i++);  // Longer delay
    uint8_t status = driver->dataport.Read(&(driver->dataport));
    status = (status | 1) & ~0x10;  // Enable keyboard interrupt

    driver->commandport.Write(&(driver->commandport), 0x60);    // Set config byte
    for(volatile int i = 0; i < 100000; i++);  // Longer delay
    driver->dataport.Write(&(driver->dataport), status);
    for(volatile int i = 0; i < 100000; i++);  // Longer delay

    // 4. Tell the keyboard to start scanning
    driver->dataport.Write(&(driver->dataport), 0xf4);
    for(volatile int i = 0; i < 100000; i++);  // Longer delay

}

void keyboard_activate(struct KeyboardDriver* driver)
{
    // Send the "Activate Scanning" command (0xF4) to the keyboard
    driver->dataport.Write(&(driver->dataport), 0xF4);
}

/*
 * Interrupt Service Routine (ISR) for the keyboard
 * This should be called from my main HandleInterrupt function in interrupts.c
 */
uint32_t handle_keyboard_interrupt(struct KeyboardDriver* driver, uint32_t esp)
{
    // Read the scancode from the data port (0x60)
    uint8_t key = driver->dataport.Read(&(driver->dataport));

    // Scan Code Set 1: check if it's a 'Make' code (Press) or 'Break' code (Release)
    // If the highest bit (0x80) is not set, the key was pressed
    if (key < 0x80)
    {
        char c = 0;
        switch (key)
        {
                // Number row
                case 0x02: c = '1'; break;
                case 0x03: c = '2'; break;
                case 0x04: c = '3'; break;
                case 0x05: c = '4'; break;
                case 0x06: c = '5'; break;
                case 0x07: c = '6'; break;
                case 0x08: c = '7'; break;
                case 0x09: c = '8'; break;
                case 0x0A: c = '9'; break;
                case 0x0B: c = '0'; break;

                // QWERTY row (adjust from your previous
                case 0x10: c = 'q'; break;
                case 0x11: c = 'w'; break;
                case 0x12: c = 'e'; break;
                case 0x13: c = 'r'; break;
                case 0x14: c = 't'; break;
                case 0x15: c = 'y'; break;
                case 0x16: c = 'u'; break;
                case 0x17: c = 'i'; break;
                case 0x18: c = 'o'; break;
                case 0x19: c = 'p'; break;

                // Home row
                case 0x1E: c = 'a'; break;
                case 0x1F: c = 's'; break;
                case 0x20: c = 'd'; break;
                case 0x21: c = 'f'; break;
                case 0x22: c = 'g'; break;
                case 0x23: c = 'h'; break;
                case 0x24: c = 'j'; break;
                case 0x25: c = 'k'; break;
                case 0x26: c = 'l'; break;

                // Bottom row
                case 0x2C: c = 'z'; break;
                case 0x2D: c = 'x'; break;
                case 0x2E: c = 'c'; break;
                case 0x2F: c = 'v'; break;
                case 0x30: c = 'b'; break;
                case 0x31: c = 'n'; break;
                case 0x32: c = 'm'; break;

                // Special Key
                case 0x1C: c = '\n'; break;
                case 0x39: c = ' '; break;

                default:
                    // Unknown scancode - ignore it
                    break;
        }
        if (c != 0 && driver->handler != 0 && driver->handler->OnKeyDown != 0)
            driver->handler->OnKeyDown(c);
    }
    else 
        if (driver->handler != 0 && driver->handler->OnKeyUp != 0)
            driver->handler->OnKeyUp(key - 0x80); // We don't have the character for key release, so we pass 0 or could enhance to track key states

    return esp;
}