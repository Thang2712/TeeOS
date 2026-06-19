#ifndef __TEEOS__COMMON__TEXT_BOX_H
#define __TEEOS__COMMON__TEXT_BOX_H

#include <common/types.h>

/*
 * VGA Text Mode Box Drawing Characters (Unicode Box Drawing)
 * These are IBM PC extended ASCII characters compatible with VGA text mode
 */
#define BOX_CHAR_HORIZONTAL      0xC4  // ─ (196)
#define BOX_CHAR_VERTICAL        0xB3  // │ (179)
#define BOX_CHAR_TOP_LEFT        0xDA  // ┌ (218)
#define BOX_CHAR_TOP_RIGHT       0xBF  // ┐ (191)
#define BOX_CHAR_BOTTOM_LEFT     0xC0  // └ (192)
#define BOX_CHAR_BOTTOM_RIGHT    0xD9  // ┘ (217)
#define BOX_CHAR_CROSS           0xC5  // ┼ (197)
#define BOX_CHAR_T_DOWN          0xC2  // ┬ (194)
#define BOX_CHAR_T_UP            0xC1  // ┴ (193)
#define BOX_CHAR_T_RIGHT         0xC3  // ├ (195)
#define BOX_CHAR_T_LEFT          0xB4  // ┤ (180)

/*
 * Text attributes for VGA text mode (foreground colors)
 */
typedef enum {
    TEXT_COLOR_BLACK        = 0x00,
    TEXT_COLOR_BLUE         = 0x01,
    TEXT_COLOR_GREEN        = 0x02,
    TEXT_COLOR_CYAN         = 0x03,
    TEXT_COLOR_RED          = 0x04,
    TEXT_COLOR_MAGENTA      = 0x05,
    TEXT_COLOR_BROWN        = 0x06,
    TEXT_COLOR_LIGHT_GREY   = 0x07,
    TEXT_COLOR_DARK_GREY    = 0x08,
    TEXT_COLOR_LIGHT_BLUE   = 0x09,
    TEXT_COLOR_LIGHT_GREEN  = 0x0A,
    TEXT_COLOR_LIGHT_CYAN   = 0x0B,
    TEXT_COLOR_LIGHT_RED    = 0x0C,
    TEXT_COLOR_LIGHT_MAGENTA= 0x0D,
    TEXT_COLOR_YELLOW       = 0x0E,
    TEXT_COLOR_WHITE        = 0x0F
} text_color_t;

/*
 * Text mode attributes (background colors)
 */
typedef enum {
    TEXT_BG_BLACK   = 0x00,
    TEXT_BG_BLUE    = 0x10,
    TEXT_BG_GREEN   = 0x20,
    TEXT_BG_CYAN    = 0x30,
    TEXT_BG_RED     = 0x40,
    TEXT_BG_MAGENTA = 0x50,
    TEXT_BG_BROWN   = 0x60,
    TEXT_BG_GREY    = 0x70
} text_bg_t;

/*
 * Draw a character at a specific position in VGA text mode
 * @param x Column (0-79)
 * @param y Row (0-24)
 * @param ch Character to draw
 * @param fg Foreground color
 * @param bg Background color
 */
void text_put_char(uint8_t x, uint8_t y, uint8_t ch, text_color_t fg, text_bg_t bg);

/*
 * Draw a string at a specific position in VGA text mode
 * @param x Column (0-79)
 * @param y Row (0-24)
 * @param str String to draw
 * @param fg Foreground color
 * @param bg Background color
 */
void text_put_string(uint8_t x, uint8_t y, const char* str, text_color_t fg, text_bg_t bg);

/*
 * Draw a horizontal line using box characters
 * @param x Starting column
 * @param y Row
 * @param width Number of characters
 * @param fg Foreground color
 * @param bg Background color
 */
void text_draw_hline(uint8_t x, uint8_t y, uint8_t width, text_color_t fg, text_bg_t bg);

/*
 * Draw a vertical line using box characters
 * @param x Column
 * @param y Starting row
 * @param height Number of characters
 * @param fg Foreground color
 * @param bg Background color
 */
void text_draw_vline(uint8_t x, uint8_t y, uint8_t height, text_color_t fg, text_bg_t bg);

/*
 * Draw a filled rectangle with border in VGA text mode
 * @param x Starting column
 * @param y Starting row
 * @param width Width of the box
 * @param height Height of the box
 * @param fg Foreground color
 * @param bg Background color
 * @param title Optional title string (centered at top)
 */
void text_draw_box(uint8_t x, uint8_t y, uint8_t width, uint8_t height, 
                   text_color_t fg, text_bg_t bg, const char* title);

/*
 * Clear the screen (fill with spaces)
 */
void text_clear_screen(void);

/*
 * Move cursor to a position
 * @param x Column (0-79)
 * @param y Row (0-24)
 */
void text_move_cursor(uint8_t x, uint8_t y);

#endif
