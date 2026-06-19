#include <common/types.h>
#include <gdt.h>
#include <multitasking.h>
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/keyboard_layout.h>
#include <drivers/keyboard_layout_display.h>
#include <drivers/vga.h>
#include <common/text_box.h>

#define MAX_CMD_LENGTH 256
#define MAX_HISTORY 10

/*
 * Global system variables
 */
int cursor_X = 0, cursor_Y = 0;
struct GlobalDescriptorTable gdt;
struct InterruptManager interrupt_man;
struct KeyboardDriver keyboard;
task_manager_t taskManager;
task_t task1, task2;
keyboard_layout_manager_t layout_manager;

// Command-line interface variables
static char command_buffer[MAX_CMD_LENGTH];
static int cmd_index = 0;
static char command_history[MAX_HISTORY][MAX_CMD_LENGTH];
static int history_index = 0;
static volatile int newline_pressed = 0;

// Forward declarations
void cli_keydown_handler(char c);
void cli_keyup_handler(char c);

// Event handler struct for the keyboard driver
static struct KeyboardEvenHandler cli_handler = {
    .OnKeyDown = &cli_keydown_handler,
    .OnKeyUp = &cli_keyup_handler
};

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
 * scroll_screen: Scrolls the screen up by one line when bottom is reached
 */
void scroll_screen()
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;
    
    // Copy each line up by one
    for(int i = 0; i < 24 * 80; i++)
    {
        VideoMemory[i] = VideoMemory[i + 80];
    }
    
    // Clear the bottom line
    for(int i = 24 * 80; i < 25 * 80; i++)
    {
        VideoMemory[i] = 0x0720; // Space with light grey on black
    }
    
    // Move cursor to start of last line
    cursor_Y = 24;
    cursor_X = 0;
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
            
            // Scroll if we've gone past the bottom
            if (cursor_Y >= 25)
            {
                scroll_screen();
            }
            continue;
        }

        // Scroll if we're at or past the bottom
        if (cursor_Y >= 25)
        {
            scroll_screen();
        }

        int location = cursor_Y * 80 + cursor_X;
        // 0x07: Light grey text on black background
        VideoMemory[location] = (uint16_t) str[i] | (0x07 << 8);

        cursor_X++;

        if (cursor_X >= 80)
        {
            cursor_X = 0;
            cursor_Y++;
            
            // Scroll if we've gone to the next line past bottom
            if (cursor_Y >= 25)
            {
                scroll_screen();
            }
        }
    }
    move_cursor(cursor_X, cursor_Y);
}

/*
 * clear_screen: Clears the VGA text mode screen
 */
void clear_screen()
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;
    for(int i = 0; i < 80 * 25; i++)
    {
        VideoMemory[i] = 0x0720; // Space character with black bg, light grey fg
    }
    cursor_X = 0;
    cursor_Y = 0;
    move_cursor(cursor_X, cursor_Y);
}

/*
 * execute_command: Parses and executes shell commands
 */
void execute_command(char* cmd)
{
    if(cmd[0] == '\0') return; // Empty command
    
    // Compare command strings
    if(cmd[0] == 'h' && cmd[1] == 'e' && cmd[2] == 'l' && cmd[3] == 'p')
    {
        kprintf("\n=== TeeOS Command Line ===\n");
        kprintf("Available commands:\n");
        kprintf("  help     - Show this help message\n");
        kprintf("  clear    - Clear the screen\n");
        kprintf("  echo     - Echo text back to console\n");
        kprintf("  sys      - Show system information\n");
        kprintf("  layout   - Display active keyboard layout\n");
        kprintf("  exit     - Halt the system\n");
    }
    else if(cmd[0] == 'c' && cmd[1] == 'l' && cmd[2] == 'e' && cmd[3] == 'a' && cmd[4] == 'r')
    {
        clear_screen();
    }
    else if(cmd[0] == 'e' && cmd[1] == 'c' && cmd[2] == 'h' && cmd[3] == 'o')
    {
        kprintf("\n");
        // Find the text after "echo " and print it
        if(cmd[4] == ' ')
        {
            int i = 5;
            while(cmd[i] != '\0')
            {
                char buf[] = " ";
                buf[0] = cmd[i];
                kprintf(buf);
                i++;
            }
            kprintf("\n");
        }
    }
    else if(cmd[0] == 's' && cmd[1] == 'y' && cmd[2] == 's')
    {
        kprintf("\n=== System Information ===\n");
        kprintf("Kernel: TeeOS v1.0\n");
        kprintf("Mode: Text (80x25)\n");
        kprintf("CPU: x86 32-bit\n");
        kprintf("Status: Running\n");
    }
    else if(cmd[0] == 'l' && cmd[1] == 'a' && cmd[2] == 'y' && cmd[3] == 'o' && cmd[4] == 'u' && cmd[5] == 't')
    {
        kprintf("\n");
        display_keyboard_layout_centered(&layout_manager);
    }
    else if(cmd[0] == 'e' && cmd[1] == 'x' && cmd[2] == 'i' && cmd[3] == 't')
    {
        kprintf("\nSystem halting...\n");
        // Halt the CPU
        asm("hlt");
    }
    else
    {
        kprintf("\nCommand not found: ");
        kprintf(cmd);
        kprintf("\nType 'help' for available commands.\n");
    }
}

/*
 * printfHex: Prints a byte value in hexadecimal
 */
void printfHex(uint8_t key)
{
    char* hex = "0123456789ABCDEF";
    char buf[] = "00";
    buf[0] = hex[(key >> 4) & 0x0F];
    buf[1] = hex[key & 0x0F];
    kprintf(buf);
}

/*
 * print_prompt: Prints the command prompt
 */
void print_prompt()
{
    kprintf("teeos> ");
}
/*
 * cli_keyup_handler: Handle key release (not used for CLI)
 */
void cli_keyup_handler(char c)
{
    // Key released - not needed for CLI
}

/*
 * cli_keydown_handler: Keyboard event handler for command-line interface
 */
void cli_keydown_handler(char c)
{
    if(c == '\n' || c == '\r')
    {
        // Enter key pressed - execute command
        command_buffer[cmd_index] = '\0';
        kprintf("\n");
        
        // Save to history
        if(cmd_index > 0 && history_index < MAX_HISTORY)
        {
            int i = 0;
            while(i < cmd_index && command_buffer[i] != '\0')
            {
                command_history[history_index][i] = command_buffer[i];
                i++;
            }
            command_history[history_index][i] = '\0';
            history_index++;
        }
        
        execute_command(command_buffer);
        cmd_index = 0;
        print_prompt();
    }
    else if(c == '\b' || c == 0x08)
    {
        // Backspace
        if(cmd_index > 0)
        {
            cmd_index--;
            // Move cursor back and erase
            if(cursor_X > 7)
            {
                cursor_X--;
                static uint16_t* VideoMemory = (uint16_t*)0xb8000;
                int location = cursor_Y * 80 + cursor_X;
                VideoMemory[location] = 0x0720;
                move_cursor(cursor_X, cursor_Y);
            }
        }
    }
    else if(c >= 32 && c < 127)
    {
        // Printable character
        if(cmd_index < MAX_CMD_LENGTH - 1)
        {
            command_buffer[cmd_index] = c;
            cmd_index++;
            char buf[] = " ";
            buf[0] = c;
            kprintf(buf);
        }
    }
}

/*
 * kernelMain: The primary entry point called by the assembly loader.
 * Command-line interface version.
 */
void kernelMain(void* multiboot_structure, uint32_t magic)
{
    // 1. Initialize GDT and Interrupts
    init_gdt(&gdt);
    clear_screen();
    
    // Initialize keyboard layout manager
    keyboard_layout_manager_init(&layout_manager);
    
    kprintf("===============================================\n");
    kprintf("      Welcome to TeeOS Command Line OS v1.0    \n");
    kprintf("===============================================\n");
    kprintf("Type 'help' for available commands.\n\n");

    // 2. Initialize Interrupt Manager
    task_manager_init(&taskManager);
    init_interrupt_manager(&interrupt_man, 0x20, &gdt, &taskManager);

    // 3. Initialize Driver Manager and Keyboard
    struct DriverManager drvManger;
    init_driver_manager(&drvManger);

    // Initialize keyboard driver with CLI handler
    init_keyboard_driver(&keyboard, &interrupt_man, &cli_handler);
    driver_manager_add_driver(&drvManger, (struct Driver*)&keyboard);
    driver_manager_activate_all(&drvManger);

    activate_interrupts();
    
    // 4. Print initial prompt
    print_prompt();
    
    // 5. Main command loop
    while(1)
    {
        asm("hlt"); // Halt CPU until next interrupt
    }
}
