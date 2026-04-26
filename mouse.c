#include "mouse.h"


void kprintf(char* str);


void init_mouse_driver(struct MouseDriver *driver, struct InterruptManager *manager)
{
    init_port8bit(&(driver->dataport), 0x60);
    init_port8bit(&(driver->commandport), 0x64);

    driver->offset = 0;
    driver->buttons = 0;
    driver->x = 40;
    driver->y = 12;

    uint16_t* VideoMemory = (uint16_t*)0xB8000;

    VideoMemory[80 * driver->y + driver->x] = (VideoMemory[80 * driver->y + driver->x] & 0x0F00) << 4
                                            | (VideoMemory[80 * driver->y + driver->x] & 0xF000) >> 4
                                            | (VideoMemory[80 * driver->y + driver->x] & 0x00FF);

    // Enable the mouse (0xA8)
    driver->commandport.Write(&(driver->commandport), 0xA8);
    for(volatile int i = 0; i < 100000; i++);  // Longer delay

    // Read PS/2 controller configuration byte
    driver->commandport.Write(&(driver->commandport), 0x20);
    for(volatile int i = 0; i < 100000; i++);  // Longer delay
    uint8_t status = driver->dataport.Read(&(driver->dataport));
    
    // Enable mouse interrupt (bit 1) and disable timeout (bit 5)
    status = (status | 0x02) & ~0x20;

    // Write back the configuration byte
    driver->commandport.Write(&(driver->commandport), 0x60);
    for(volatile int i = 0; i < 100000; i++);  // Longer delay
    driver->dataport.Write(&(driver->dataport), status);
    for(volatile int i = 0; i < 100000; i++);  // Longer delay

    // Enable mouse scanning (0xF4)
    driver->commandport.Write(&(driver->commandport), 0xD4);
    for(volatile int i = 0; i < 100000; i++);  // Longer delay
    driver->dataport.Write(&(driver->dataport), 0xF4);
    for(volatile int i = 0; i < 100000; i++);  // Longer delay
};



uint32_t handle_mouse_interrupt(struct MouseDriver *driver, uint32_t esp)
{
        uint8_t status = driver->commandport.Read(&(driver->commandport));
        if (!(status & 0x01))
            return esp;

        driver->buffer[driver->offset] = driver->dataport.Read(&(driver->dataport));
        driver->offset = (driver->offset + 1) % 3;

        if (driver->offset == 0)
        {
            if (driver->buffer[1] != 0 || driver->buffer[2] != 0)
            {
                static uint16_t* VideoMemory = (uint16_t*)0xB8000;

                VideoMemory[80 * driver->y + driver->x] = (VideoMemory[80 * driver->y + driver->x] & 0x0F00) << 4
                                                        | (VideoMemory[80 * driver->y + driver->x] & 0xF000) >> 4
                                                        | (VideoMemory[80 * driver->y + driver->x] & 0x00FF);

                driver->x += driver->buffer[1];
                if (driver->x >= 80)
                    driver->x = 79;
                if (driver->x < 0)
                    driver->x = 0;

                driver->y -= driver->buffer[2];
                if (driver->y >= 25)
                    driver->y = 24;
                if (driver->y < 0)
                    driver->y = 0;

                VideoMemory[80 * driver->y + driver->x] = (VideoMemory[80 * driver->y + driver->x] & 0x0F00) << 4
                                                        | (VideoMemory[80 * driver->y + driver->x] & 0xF000) >> 4
                                                        | (VideoMemory[80 * driver->y + driver->x] & 0x00FF);
            }

            {
                uint8_t prev = driver->buttons;
                uint8_t btn = driver->buffer[0];

                if ((btn & 0x01) && !(prev & 0x01))
                    kprintf("LEFT DOWN\n");
                if (!(btn & 0x01) && (prev & 0x01))
                    kprintf("LEFT UP\n");

                if ((btn & 0x02) && !(prev & 0x02))
                    kprintf("RIGHT DOWN\n");
                if (!(btn & 0x02) && (prev & 0x02))
                    kprintf("RIGHT UP\n");

                if ((btn & 0x04) && !(prev & 0x04))
                    kprintf("MIDDLE DOWN\n");
                if (!(btn & 0x04) && (prev & 0x04))
                    kprintf("MIDDLE UP\n");

                driver->buttons = btn;
            }
        }

    return esp;
}