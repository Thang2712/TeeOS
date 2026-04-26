#include "keyboard.h"

void kprintf(char *str);

/*
 * Initializes the Keyboard Driver.
 * @param driver: Pointer to the KeyboardDriver struct instance.
 * @param manager: Pointer to the InterruptManager (for remapping IRQ1)
 */
void init_keyboard_driver(struct KeyboardDriver* driver, struct InterruptManager* manager)
{
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
        switch (key)
        {
                // Number row
                case 0x02: kprintf("1"); break;
                case 0x03: kprintf("2"); break;
                case 0x04: kprintf("3"); break;
                case 0x05: kprintf("4"); break;
                case 0x06: kprintf("5"); break;
                case 0x07: kprintf("6"); break;
                case 0x08: kprintf("7"); break;
                case 0x09: kprintf("8"); break;
                case 0x0A: kprintf("9"); break;
                case 0x0B: kprintf("0"); break;

                // QWERTY row (adjust from your previous
                case 0x10: kprintf("q"); break;
                case 0x11: kprintf("w"); break;
                case 0x12: kprintf("e"); break;
                case 0x13: kprintf("r"); break;
                case 0x14: kprintf("t"); break;
                case 0x15: kprintf("y"); break;
                case 0x16: kprintf("u"); break;
                case 0x17: kprintf("i"); break;
                case 0x18: kprintf("o"); break;
                case 0x19: kprintf("p"); break;

                // Home row
                case 0x1E: kprintf("a"); break;
                case 0x1F: kprintf("s"); break;
                case 0x20: kprintf("d"); break;
                case 0x21: kprintf("f"); break;
                case 0x22: kprintf("g"); break;
                case 0x23: kprintf("h"); break;
                case 0x24: kprintf("j"); break;
                case 0x25: kprintf("k"); break;
                case 0x26: kprintf("l"); break;

                // Bottom row
                case 0x2C: kprintf("z"); break;
                case 0x2D: kprintf("x"); break;
                case 0x2E: kprintf("c"); break;
                case 0x2F: kprintf("v"); break;
                case 0x30: kprintf("b"); break;
                case 0x31: kprintf("n"); break;
                case 0x32: kprintf("m"); break;

                // Special Key
                case 0x1C: kprintf("\n"); break;
                case 0x39: kprintf(" "); break;


                default:
                {
                    // Fallback for unknown scancodes: Print the hex values
                    static char* hex = "0123456789ABCDEF";
                    kprintf("[0x");
                    char lo = hex[key & 0x0F];
                    char hi = hex[(key >> 4) & 0x0F];
                    // Manually printing hex to avoid complex printf logic
                    char out[3] = {hi, lo, '\0'};
                    kprintf(out);
                    kprintf("]");
                    break;
                }
        }
    }

    return esp;
}