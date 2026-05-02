#include <drivers/mouse.h>


void kprintf(char* str);

/*
 * @brief Simple buse-wait delay.
 * Required to give the PS/2 controller time to process command
 */
static void mouse_delay(void)
{
    for (volatile uint32_t i = 0; i < 500000; i++);
}

/*
 * @brief Waits for the input buffer to be full (Data ready to be read).
 * Checks bit 0 of the Status Register (0x64)
 */
static int mouse_wait_read(struct MouseDriver *driver)
{
    for (uint32_t i = 0; i < 1000000; i++)
        if (driver->commandport.Read(&(driver->commandport)) & 0x01)
            return 1;
    return 0;
}

/*
 * @brief Waits for the input buffer to be empty (Ready to send data).
 * Checks bit 1 of the Status Register (0x64)
 */
static int mouse_wait_write(struct MouseDriver *driver)
{
    for (uint32_t i = 0; i < 1000000; i++)
        if (!(driver->commandport.Read(&(driver->commandport)) & 0x02))
            return 1;
    return 0;
}

static uint8_t mouse_read_data(struct MouseDriver *driver)
{
    if (!mouse_wait_read(driver))
        return 0;
    return driver->dataport.Read(&(driver->dataport));
}

/*
 * @brief Sends a command byte specifically to the auxiliary PS/2 device (Mouse).
 * Uses command 0xD4 to tell the controller the next byte is for the mouse.
 */
static void mouse_write_device(struct MouseDriver *driver, uint8_t value)
{
    if (!mouse_wait_write(driver))
        return;
    driver->commandport.Write(&(driver->commandport), 0xD4);
    if (!mouse_wait_write(driver))
        return;
    driver->dataport.Write(&(driver->dataport), value);
}

/*
 * @brief Initializes the PS/2 Mouse hardware and driver state
 * Configures the 8042 controller, enables IRQ12, and sets mouse parameters.
 */
void init_mouse_driver(struct MouseDriver *driver, struct InterruptManager *manager, struct MouseEventHandler* handler)
{
    init_port8bit(&(driver->dataport), 0x60);
    init_port8bit(&(driver->commandport), 0x64);

    driver->handler = handler;
    driver->offset = 0;
    driver->buttons = 0;
    driver->x = 40; // Initial center X
    driver->y = 12; // Initial center Y
    driver->click_feedback_counter = 0;

    // Set the interrupt handler callback
    driver->base.Handle = (uint32_t (*)(struct InterruptHandler*, uint32_t)) handle_mouse_interrupt;

    // Initial cursor draw (inverts colors at starting position)
    uint16_t* VideoMemory = (uint16_t*)0xB8000;
    VideoMemory[80 * driver->y + driver->x] = (VideoMemory[80 * driver->y + driver->x] & 0x0F00) << 4
                                            | (VideoMemory[80 * driver->y + driver->x] & 0xF000) >> 4
                                            | (VideoMemory[80 * driver->y + driver->x] & 0x00FF);

    // Drain stale controller data before configuration.
    while (driver->commandport.Read(&(driver->commandport)) & 0x01)
        driver->dataport.Read(&(driver->dataport));

    // Controller Configuration
    // Enable secondary PS/2 port
    if (mouse_wait_write(driver))
        driver->commandport.Write(&(driver->commandport), 0xA8);
    mouse_delay();

    // Read/modify/write PS/2 controller config: enable IRQ12.
    if (mouse_wait_write(driver))
        driver->commandport.Write(&(driver->commandport), 0x20);
    mouse_delay();
    uint8_t status = mouse_read_data(driver);   // Set IRQ12 bit
    status |= 0x02;
    status &= ~0x20; // Clear disable-mouse bit


    if (mouse_wait_write(driver))
        driver->commandport.Write(&(driver->commandport), 0x60);
    mouse_delay();
    if (mouse_wait_write(driver))
        driver->dataport.Write(&(driver->dataport), status);
    mouse_delay();

    // Set sane defaults then enable streaming mode.
    mouse_write_device(driver, 0xF6);  // Set Default Settings
    mouse_delay();

    // Set sample rate to 40 Hz for better precision and less erratic behavior
    mouse_write_device(driver, 0xF3);  // Set Sample Rate
    mouse_delay();
    mouse_write_device(driver, 100);    // 100 Hz sample rate (lower = less erratic)
    mouse_delay();

    // Set resolution to 1 count/mm for reduced sensitivity
    mouse_write_device(driver, 0xE8);  // Set Resolution
    mouse_delay();
    mouse_write_device(driver, 0);     // 1 count/mm (0=1, 1=2, 2=4, 3=8) - lowest sensitivity
    mouse_delay();

    mouse_write_device(driver, 0xF4);  // Enable Data Reporting
    mouse_delay();

    if (driver->handler != 0 && driver->handler->OnActivate != 0)
        driver->handler->OnActivate();
};


/**
 * @brief Handles interrupts from the PS/2 Mouse.
 * Process 3-byte packets and dispatches events to the MouseEventHandler.
 */
uint32_t handle_mouse_interrupt(struct MouseDriver *driver, uint32_t esp)
{
    // Read status from the PS/2 controller status register (Port 0x64)
    uint8_t status = driver->commandport.Read(&(driver->commandport));
    
    // Check if the data is available (bit 0) and if it comes from the mouse (bit 5)
    // 0x01: Output buffer full, 0x20: Auxiliary device (mouse) data
    if (!(status & 0x20) || !(status & 0x01))
        return esp;

    // Read the actual data byte from the data port (Port 0x60)
    uint8_t data = driver->dataport.Read(&(driver->dataport));

    // Store the byte into the driver's circular buffer
    driver->buffer[driver->offset] = data;

    // If no handler is registered, we just consume the data and return
    if (driver->handler == 0)
        return esp;

    // Increment offset to prepare for the next byte in the 3-byte sequence
    driver->offset = (driver->offset + 1) % 3;

    // Once a complete 3-byte packet is received
    if (driver->offset == 0)
    {
        // Valid PS/2 mouse packets should always have bit 3 of the first byte set to 1
        if (!(driver->buffer[0] & 0x08))
            return esp;

        // Check if there's any movement (buffer[1] is X-delta, buffer[2] is Y-delta)
        if (driver->buffer[1] != 0 || driver->buffer[2] != 0)
        {
            // Cast to int8_t to handle signed relative movement correctly.
            // Note: We negate the Y-axis movement because PS/2 'up' is positive, 
            // while screen 'down' is positive.
            if (driver->handler->OnMouseMove != 0)
            {
                driver->handler->OnMouseMove((int8_t)driver->buffer[1], -((int8_t)driver->buffer[2]));
            }
        }

        // Process button states (Left, Right, and Middle buttons)
        for (uint8_t i = 0; i < 3; i++)
        {
            uint8_t mask = 0x1 << i;
            // Check if the button state has changed compared to the last recorded state
            if ((driver->buffer[0] & mask) != (driver->buttons & mask))
            {
                // If the bit is now 1, the button was pressed
                if (driver->buffer[0] & mask)
                {
                    if (driver->handler->OnMouseDown != 0)
                        driver->handler->OnMouseDown(i + 1);
                }
                // If the bit is now 0, the button was released
                else
                {
                    if (driver->handler->OnMouseUp != 0)
                        driver->handler->OnMouseUp(i + 1);
                }
            }
        }

        // Update the driver's button state for the next interrupt comparison
        driver->buttons = driver->buffer[0];
    }

    return esp;
}