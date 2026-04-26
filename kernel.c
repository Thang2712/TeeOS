#include "types.h"
#include "gdt.h"
#include "port.h"
#include "interrupts.h"
#include "keyboard.h"

/*
 * VGA test mode constants:
 * Screen is 80 columns wide and 25 rows high.
 * Video memory starts at physical address 0xB8000
 */
int cursor_X = 0, cursor_Y = 0;
struct GlobalDescriptorTable gdt;
struct InterruptManager interrupt_man;
struct KeyboardDriver keyboard;

/*
 * move_cursor: communicates with the VGA hardware to update the
 * blinking underscore on the screen.
 * @param x: column coordinate (0-79)
 * @param y: row coordinate (0-24)
 */
void move_cursor(int x, int y)
{
    // Calculate the 1-dimensional position in the video buffer
    uint16_t pos = y * 80 + x;

    struct Port8Bit controlPort;
    struct Port8Bit dataPort;

    // 0x3D4 is the CRT Controller Index Register
    // 0x3D5 is the CRT Controller Data Register
    init_port8bit(&controlPort, 0x3D4);
    init_port8bit(&dataPort, 0x3D5);

    // Tell VGA we are sending the low byte of the cursor position (Register 0x0F)
    controlPort.Write(&controlPort, 0x0F);
    dataPort.Write(&dataPort, (uint8_t) (pos & 0xFF));

    // Tell VGA we are sending the high byte of the cursor position (Register 0x0E)
    controlPort.Write(&controlPort, 0x0E);
    dataPort.Write(&dataPort, (uint8_t) ((pos >> 8) & 0xFF));
}

/*
 * kprintf: Kernel Print Function.
 * Writes strings directly to VGA memory and handles basic newline (\n) logic.
 */
void kprintf(char* str)
{
    // VGA buffer uses 2 bytes per character: [Attribute Byte][Character Byte]
    // Attribute 0x07 = Light Grey text on Black background
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    for (int i = 0; str[i] != '\0'; i++)
    {
        // Handle Newline: reset X to start of line and increment Y
        if (str[i] == '\n')
        {
            cursor_X = 0;
            cursor_Y++;
            continue;
        }

        // Calculate buffer index and write the 16-bit character/color pair
        int location = cursor_Y * 80 + cursor_X;
        VideoMemory[location] = (uint16_t) str[i] | (0x07 << 8);

        cursor_X++;

        // Automatic word wrap if text exceeds the screen width
        if (cursor_X >= 80)
        {
            cursor_X = 0;
            cursor_Y++;
        }
    }

    // sync the hardware cursor with our software coordinate
    move_cursor(cursor_X, cursor_Y);
}

/*
 * kernelMain: the primary C entry point called by the assembly loader
 * @param multiboot_structure: pointer to info provided by the bootloader (GRUB)
 * @param magic: magic number used to verify multiboot compliance
 */
void kernelMain(void* multiboot_structure, uint32_t magic)
{
    // Initialize the Global Descriptor Table for memory protection/segmentation
    init_gdt(&gdt);

    // Output boot messages to the screen
    kprintf("Hello World --- Mr.Teejaze\n");
    kprintf("GDT initialized successfully\n");


    init_interrupt_manager(&interrupt_man, 0x20, &gdt);
    kprintf("Initializing Keyboard Driver...\n");
    init_keyboard_driver(&keyboard, &interrupt_man);


    activate_interrupts();
    kprintf("Interrupts activated. System already\n");
    // enter an infinite loop to keep the CPU from executing garbage memory
    while(1);
}

