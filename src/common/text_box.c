#include <common/text_box.h>

// Define NULL for freestanding environment
#ifndef NULL
#define NULL ((void*)0)
#endif

// VGA Video memory address for text mode
static uint16_t* video_memory = (uint16_t*)0xB8000;

/*
 * Draw a character at a specific position in VGA text mode
 */
void text_put_char(uint8_t x, uint8_t y, uint8_t ch, text_color_t fg, text_bg_t bg)
{
    if (x >= 80 || y >= 25)
        return;
    
    uint16_t offset = y * 80 + x;
    uint16_t attribute = ((bg << 4) | fg);
    video_memory[offset] = (attribute << 8) | ch;
}

/*
 * Draw a string at a specific position in VGA text mode
 */
void text_put_string(uint8_t x, uint8_t y, const char* str, text_color_t fg, text_bg_t bg)
{
    if (!str || x >= 80 || y >= 25)
        return;
    
    uint8_t curr_x = x;
    uint8_t curr_y = y;
    
    for (int i = 0; str[i] != '\0' && curr_x < 80; i++) {
        if (str[i] == '\n') {
            curr_x = x;
            curr_y++;
            if (curr_y >= 25)
                break;
        } else {
            text_put_char(curr_x, curr_y, (uint8_t)str[i], fg, bg);
            curr_x++;
        }
    }
}

/*
 * Draw a horizontal line using box characters
 */
void text_draw_hline(uint8_t x, uint8_t y, uint8_t width, text_color_t fg, text_bg_t bg)
{
    if (x + width > 80 || y >= 25)
        return;
    
    for (uint8_t i = 0; i < width; i++) {
        text_put_char(x + i, y, BOX_CHAR_HORIZONTAL, fg, bg);
    }
}

/*
 * Draw a vertical line using box characters
 */
void text_draw_vline(uint8_t x, uint8_t y, uint8_t height, text_color_t fg, text_bg_t bg)
{
    if (x >= 80 || y + height > 25)
        return;
    
    for (uint8_t i = 0; i < height; i++) {
        text_put_char(x, y + i, BOX_CHAR_VERTICAL, fg, bg);
    }
}

/*
 * Draw a filled rectangle with border in VGA text mode
 */
void text_draw_box(uint8_t x, uint8_t y, uint8_t width, uint8_t height, 
                   text_color_t fg, text_bg_t bg, const char* title)
{
    if (x + width > 80 || y + height > 25)
        return;
    
    // Draw top border with title
    text_put_char(x, y, BOX_CHAR_TOP_LEFT, fg, bg);
    
    if (title != NULL) {
        // Calculate title position (centered)
        int title_len = 0;
        while (title[title_len] != '\0')
            title_len++;
        
        int title_start = x + 2;
        
        // Draw line up to title
        if (title_start > x + 1) {
            for (uint8_t i = 1; i < title_start - x; i++) {
                text_put_char(x + i, y, BOX_CHAR_HORIZONTAL, fg, bg);
            }
        }
        
        // Draw title
        text_put_string(title_start, y, title, fg, bg);
        
        // Draw line after title
        uint8_t title_end = title_start + title_len;
        if (title_end < x + width - 1) {
            for (uint8_t i = title_end; i < x + width - 1; i++) {
                text_put_char(i, y, BOX_CHAR_HORIZONTAL, fg, bg);
            }
        }
    } else {
        // Draw top line without title
        for (uint8_t i = 1; i < width - 1; i++) {
            text_put_char(x + i, y, BOX_CHAR_HORIZONTAL, fg, bg);
        }
    }
    
    text_put_char(x + width - 1, y, BOX_CHAR_TOP_RIGHT, fg, bg);
    
    // Draw middle rows
    for (uint8_t row = y + 1; row < y + height - 1; row++) {
        // Left border
        text_put_char(x, row, BOX_CHAR_VERTICAL, fg, bg);
        
        // Fill inside with spaces
        for (uint8_t col = x + 1; col < x + width - 1; col++) {
            text_put_char(col, row, ' ', fg, bg);
        }
        
        // Right border
        text_put_char(x + width - 1, row, BOX_CHAR_VERTICAL, fg, bg);
    }
    
    // Draw bottom border
    text_put_char(x, y + height - 1, BOX_CHAR_BOTTOM_LEFT, fg, bg);
    for (uint8_t i = 1; i < width - 1; i++) {
        text_put_char(x + i, y + height - 1, BOX_CHAR_HORIZONTAL, fg, bg);
    }
    text_put_char(x + width - 1, y + height - 1, BOX_CHAR_BOTTOM_RIGHT, fg, bg);
}

/*
 * Clear the screen (fill with spaces)
 */
void text_clear_screen(void)
{
    for (int i = 0; i < 80 * 25; i++) {
        video_memory[i] = 0x0720;  // Space with light grey on black
    }
}

/*
 * Move cursor to a position
 */
void text_move_cursor(uint8_t x, uint8_t y)
{
    if (x >= 80 || y >= 25)
        return;
    
    uint16_t pos = y * 80 + x;
    
    struct Port8Bit {
        uint16_t port_number;
    };
    
    // Use inline assembly to set cursor position
    uint16_t controlPort = 0x3D4;
    uint16_t dataPort = 0x3D5;
    
    // Set cursor low byte
    asm volatile (
        "movw $0x3D4, %%dx\n"
        "movb $0x0F, %%al\n"
        "outb %%al, %%dx\n"
        "movw $0x3D5, %%dx\n"
        "movb %%bl, %%al\n"
        "outb %%al, %%dx\n"
        : : "b" (pos & 0xFF)
        : "%al", "%dx"
    );
    
    // Set cursor high byte
    asm volatile (
        "movw $0x3D4, %%dx\n"
        "movb $0x0E, %%al\n"
        "outb %%al, %%dx\n"
        "movw $0x3D5, %%dx\n"
        "movb %%bl, %%al\n"
        "outb %%al, %%dx\n"
        : : "b" ((pos >> 8) & 0xFF)
        : "%al", "%dx"
    );
}
