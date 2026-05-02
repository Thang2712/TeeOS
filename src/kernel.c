#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <gui/widget.h>
#include <gui/desktop.h>
#include <gui/window.h>

/*
 * Global system variables
 */
int cursor_X = 0, cursor_Y = 0;
struct GlobalDescriptorTable gdt;
struct InterruptManager interrupt_man;
struct KeyboardDriver keyboard;
struct MouseDriver mouse;
desktop_t desktop;

/*
 * move_cursor: Updates the blinking hardware cursor position on the VGA text console.
 */
void move_cursor(int x, int y)
{
    uint16_t pos = y * 80 + x;
    struct Port8Bit controlPort;
    struct Port8Bit dataPort;

    init_port8bit(&controlPort, 0x3D4); // CRT Controller Index Register
    init_port8bit(&dataPort, 0x3D5);    // CRT Controller Data Register

    controlPort.Write(&controlPort, 0x0F);
    dataPort.Write(&dataPort, (uint8_t) (pos & 0xFF));

    controlPort.Write(&controlPort, 0x0E);
    dataPort.Write(&dataPort, (uint8_t) ((pos >> 8) & 0xFF));
}

/*
 * kprintf: Kernel Print Function. 
 * Directly writes strings to VGA video memory (0xB8000).
 */
void kprintf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\n')
        {
            cursor_X = 0;
            cursor_Y++;
            continue;
        }

        int location = cursor_Y * 80 + cursor_X;
        // 0x07: Light grey text on black background
        VideoMemory[location] = (uint16_t) str[i] | (0x07 << 8);

        cursor_X++;

        if (cursor_X >= 80)
        {
            cursor_X = 0;
            cursor_Y++;
        }
    }
    move_cursor(cursor_X, cursor_Y);
}

void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0x0F];
    foo[1] = hex[key & 0x0F];
    kprintf(foo); 
}

/*
 * my_keydown_handler: Keyboard event handler for text mode.
 */
void my_keydown_handler(char c)
{
    char buf[] = " "; 
    buf[0] = c;
    kprintf(buf);
}

/*
 * my_mousemove_handler: Mouse event handler for text mode (inverts character colors).
 */
static int mouse_X = 40, mouse_Y = 12;
void my_mousemove_handler(int8_t xoffset, int8_t yoffset)
{
    uint16_t *VideoMemory = (uint16_t*)0xB8000;

    // Invert colors at current position
    VideoMemory[80 * mouse_Y + mouse_X] = (VideoMemory[80 * mouse_Y + mouse_X] & 0x0F00) << 4
                                        | (VideoMemory[80 * mouse_Y + mouse_X] & 0xF000) >> 4
                                        | (VideoMemory[80 * mouse_Y + mouse_X] & 0x00FF);
    
    mouse_X += xoffset;
    if (mouse_X < 0) mouse_X = 0;
    if (mouse_X >= 80) mouse_X = 79;

    mouse_Y += yoffset;
    if (mouse_Y < 0) mouse_Y = 0;
    if (mouse_Y >= 25) mouse_Y = 24;

    // Invert colors at new position
    VideoMemory[80 * mouse_Y + mouse_X] = (VideoMemory[80 * mouse_Y + mouse_X] & 0x0F00) << 4
                                        | (VideoMemory[80 * mouse_Y + mouse_X] & 0xF000) >> 4
                                        | (VideoMemory[80 * mouse_Y + mouse_X] & 0x00FF);
}

/*
 * kernelMain: The primary entry point called by the assembly loader.
 */
void kernelMain(void* multiboot_structure, uint32_t magic)
{
    // 1. Initialize GDT and Interupts
    init_gdt(&gdt);
    kprintf("Hello World --- Mr.Teejaze\n");
    kprintf("GDT initialized successfully\n");

    init_interrupt_manager(&interrupt_man, 0x20, &gdt);

    // 2. Initialize Driver Manager and VGA
    struct DriverManager drvManger;
    init_driver_manager(&drvManger);

    vga_driver_t vga;
    vga_init(&vga);

    // 3. Initialize Input Drivers with default text mode handlers
    kprintf("Initializing Keyboard Driver...\n");
    static struct KeyboardEvenHandler kb_handler;
    kb_handler.OnKeyDown = &my_keydown_handler;
    init_keyboard_driver(&keyboard, &interrupt_man, &kb_handler);
    driver_manager_add_driver(&drvManger, (struct Driver*)&keyboard);

    kprintf("Initializing Mouse Driver...\n");
    static struct MouseEventHandler m_handler;
    m_handler.OnMouseMove = my_mousemove_handler;
    init_mouse_driver(&mouse, &interrupt_man, &m_handler);
    driver_manager_add_driver(&drvManger, (struct Driver*)&mouse);

    // 4. PCI Bus Scanning
    kprintf("Scanning PCI Bus .............\n");
    pci_controller_t pci;
    pci_init(&pci);
    pci_select_drivers(&pci, &drvManger, &interrupt_man);

    // 5. Activate all hardware drivers
    kprintf("Activating Hardware Drivers......\n");
    driver_manager_activate_all(&drvManger);

    // 6. Switch to VGA Graphics Mode 13h (320x200, 8-bit color)
    if (vga_set_mode(&vga, 320, 200, 8))
    {
        // Initialize Desktop GUI
        desktop_init(&desktop, 320, 200, 0x00, 0x00, 0xA8);

        // Switch event handlers to Desktop for GUI processing
        init_keyboard_driver(&keyboard, &interrupt_man, (struct KeyboardEvenHandler*)&desktop);
        init_mouse_driver(&mouse, &interrupt_man, (struct MouseEventHandler*)&desktop);

        // Initialize sample windows
        static window_t win1;
        window_init(&win1, (widget_t*)&desktop, 20, 20, 60, 40, 0xA8, 0x00, 0x00);
        composite_widget_add_child((composite_widget_t*)&desktop, (widget_t*)&win1);

        static window_t win2;
        window_init(&win2, (widget_t*)&desktop, 100, 50, 80, 60, 0x00, 0xA8, 0x00);
        composite_widget_add_child((composite_widget_t*)&desktop, (widget_t*)&win2);

        activate_interrupts(); 
        while (1)
            desktop.base.base.draw((widget_t*)&desktop, (graphics_context_t*) &vga);
    }
}