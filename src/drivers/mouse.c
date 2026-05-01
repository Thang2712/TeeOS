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



uint32_t handle_mouse_interrupt(struct MouseDriver *driver, uint32_t esp)
{
        uint8_t status = driver->commandport.Read(&(driver->commandport));
        if (!(status & 0x01))
            return esp;     // No data available

        uint8_t data = driver->dataport.Read(&(driver->dataport));
        if (driver->offset == 0 && !(data & 0x08))
            return esp;

        // Read byte and store in 3-byte buffer
        driver->buffer[driver->offset] = driver->dataport.Read(&(driver->dataport));
        driver->offset = (driver->offset + 1) % 3;

        // Once a full 3-byte packet is received
        if (driver->offset == 0)
        {
            if (!(driver->buffer[0] & 0x08))
                return esp;
            
            // Decrement click feedback timer
            if (driver->click_feedback_counter > 0)
                driver->click_feedback_counter--;

            if (driver->buffer[1] != 0 || driver->buffer[2] != 0)
            {
                static uint16_t* VideoMemory = (uint16_t*)0xB8000;

                // Only erase old cursor if not in click feedback mode
                if (driver->click_feedback_counter == 0)
                {
                    VideoMemory[80 * driver->y + driver->x] = (VideoMemory[80 * driver->y + driver->x] & 0x0F00) << 4
                                                            | (VideoMemory[80 * driver->y + driver->x] & 0xF000) >> 4
                                                            | (VideoMemory[80 * driver->y + driver->x] & 0x00FF);
                }

                // Get movement deltas
                int8_t dx = (int8_t) driver->buffer[1];
                int8_t dy = (int8_t) driver->buffer[2];
                
                // Moderate movement scale for precision and range (divide by 4)
                int8_t scaled_dx = dx / 4;
                int8_t scaled_dy = dy / 4;
                
                int8_t unit_dx = 0;
                int8_t unit_dy = 0;
                
                if (scaled_dx > 0) unit_dx = 1;
                else if (scaled_dx < 0) unit_dx = -1;
                
                if (scaled_dy > 0) unit_dy = 1;
                else if (scaled_dy < 0) unit_dy = -1;

                // Move cursor with CORRECTED coordinate mapping:
                // X: subtraction (PS/2 positive = right, but we negate to correct inversion)
                // Y: addition (PS/2 positive = up physically, maps to down in text mode)
                int new_x = driver->x + (dx/4);
                int new_y = driver->y - (dy/4);
                
                // Clamp to screen boundaries
                if (new_x < 0) new_x = 0;
                if (new_x > 79) new_x = 79;
                if (new_y < 0) new_y = 0;
                if (new_y > 24) new_y = 24;
                
                driver->x = new_x;
                driver->y = new_y;

                // Draw cursor only if not in click feedback mode
                if (driver->click_feedback_counter == 0)
                {
                    VideoMemory[80 * driver->y + driver->x] = (VideoMemory[80 * driver->y + driver->x] & 0x0F00) << 4
                                                            | (VideoMemory[80 * driver->y + driver->x] & 0xF000) >> 4
                                                            | (VideoMemory[80 * driver->y + driver->x] & 0x00FF);
                }

                if (driver->click_feedback_counter > 0) 
                    driver->click_feedback_counter--;
                if (driver->handler != 0 && driver->handler->OnMouseMove != 0)
                    driver->handler->OnMouseMove(dx/4, -(dy/4));
            }
            uint8_t prev = driver->buttons;
            uint8_t btn = driver->buffer[0]; 
            for (uint8_t i = 0; i < 3; i++)
            {
                if ((btn & (0x1 << i)) && !(prev & (1 << i)))
                    if (btn & (0x1 << i))
                    {
                        driver->click_feedback_counter = 80; 

                        if (driver->handler != 0 && driver->handler->OnMouseDown)
                            driver->handler->OnMouseDown(i + 1);
                    }
                else 
                {
                    if (driver->handler != 0 && driver->handler->OnMouseUp)
                        driver->handler->OnMouseUp(i + 1);
                }

            }

                driver->buttons = btn;
            }
            return esp;
        }
