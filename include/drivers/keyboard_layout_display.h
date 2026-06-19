#ifndef __TEEOS__DRIVERS__KEYBOARD_LAYOUT_DISPLAY_H
#define __TEEOS__DRIVERS__KEYBOARD_LAYOUT_DISPLAY_H

#include <drivers/keyboard_layout.h>
#include <common/text_box.h>

/*
 * Display an active keyboard layout box in the center of the screen
 * @param manager Pointer to the keyboard layout manager
 * @param x Starting column for the box
 * @param y Starting row for the box
 * @param width Width of the display box
 * @param height Height of the display box
 */
void display_keyboard_layout_box(keyboard_layout_manager_t* manager, 
                                 uint8_t x, uint8_t y, 
                                 uint8_t width, uint8_t height);

/*
 * Display a centered keyboard layout box
 * @param manager Pointer to the keyboard layout manager
 */
void display_keyboard_layout_centered(keyboard_layout_manager_t* manager);

#endif
