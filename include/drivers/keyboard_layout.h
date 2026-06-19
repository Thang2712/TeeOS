#ifndef __TEEOS__DRIVERS__KEYBOARD_LAYOUT_H
#define __TEEOS__DRIVERS__KEYBOARD_LAYOUT_H

#include <common/types.h>

/*
 * Keyboard Layout Types
 */
typedef enum {
    LAYOUT_US_QWERTY,
    LAYOUT_US_DVORAK,
    LAYOUT_COLEMAK,
    LAYOUT_COUNT
} keyboard_layout_type;

/*
 * Keyboard Layout Manager Structure
 */
typedef struct {
    keyboard_layout_type current_layout;
    char layout_names[LAYOUT_COUNT][16];
} keyboard_layout_manager_t;

/*
 * Initialize the keyboard layout manager with default layout (US QWERTY)
 * @param manager Pointer to the keyboard layout manager
 */
void keyboard_layout_manager_init(keyboard_layout_manager_t* manager);

/*
 * Get the name of the current layout
 * @param manager Pointer to the keyboard layout manager
 * @return Pointer to the layout name string
 */
const char* keyboard_layout_get_current_name(keyboard_layout_manager_t* manager);

/*
 * Switch to the next keyboard layout
 * @param manager Pointer to the keyboard layout manager
 */
void keyboard_layout_next(keyboard_layout_manager_t* manager);

/*
 * Switch to the previous keyboard layout
 * @param manager Pointer to the keyboard layout manager
 */
void keyboard_layout_previous(keyboard_layout_manager_t* manager);

/*
 * Set the keyboard layout to a specific type
 * @param manager Pointer to the keyboard layout manager
 * @param layout The layout type to set
 */
void keyboard_layout_set(keyboard_layout_manager_t* manager, keyboard_layout_type layout);

#endif
