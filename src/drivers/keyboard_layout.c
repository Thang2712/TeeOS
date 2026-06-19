#include <drivers/keyboard_layout.h>

/*
 * Initialize the keyboard layout manager with default layout (US QWERTY)
 */
void keyboard_layout_manager_init(keyboard_layout_manager_t* manager)
{
    manager->current_layout = LAYOUT_US_QWERTY;
    
    // Initialize layout names
    const char* names[] = {
        "US QWERTY",
        "US Dvorak",
        "Colemak"
    };
    
    for (int i = 0; i < LAYOUT_COUNT; i++) {
        int j = 0;
        while (names[i][j] != '\0' && j < 15) {
            manager->layout_names[i][j] = names[i][j];
            j++;
        }
        manager->layout_names[i][j] = '\0';
    }
}

/*
 * Get the name of the current layout
 */
const char* keyboard_layout_get_current_name(keyboard_layout_manager_t* manager)
{
    return manager->layout_names[manager->current_layout];
}

/*
 * Switch to the next keyboard layout
 */
void keyboard_layout_next(keyboard_layout_manager_t* manager)
{
    manager->current_layout = (manager->current_layout + 1) % LAYOUT_COUNT;
}

/*
 * Switch to the previous keyboard layout
 */
void keyboard_layout_previous(keyboard_layout_manager_t* manager)
{
    manager->current_layout = (manager->current_layout == 0) ? (LAYOUT_COUNT - 1) : (manager->current_layout - 1);
}

/*
 * Set the keyboard layout to a specific type
 */
void keyboard_layout_set(keyboard_layout_manager_t* manager, keyboard_layout_type layout)
{
    if (layout < LAYOUT_COUNT) {
        manager->current_layout = layout;
    }
}
