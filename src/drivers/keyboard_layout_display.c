#include <drivers/keyboard_layout_display.h>

/*
 * Helper function to convert integer to string
 */
static void int_to_string(int num, char* str, int max_len)
{
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    int temp = num;
    int len = 0;
    while (temp > 0) {
        temp /= 10;
        len++;
    }
    
    if (len >= max_len) len = max_len - 1;
    
    str[len] = '\0';
    temp = num;
    for (int i = len - 1; i >= 0 && temp > 0; i--) {
        str[i] = '0' + (temp % 10);
        temp /= 10;
    }
}

/*
 * Helper function to get string length
 */
static int string_length(const char* str)
{
    int len = 0;
    while (str[len] != '\0')
        len++;
    return len;
}

/*
 * Display US QWERTY keyboard layout
 */
static void display_qwerty_layout(uint8_t x, uint8_t y)
{
    // Row 1 - Number row
    const char* row1 = "1 2 3 4 5 6 7 8 9 0";
    text_put_string(x + 2, y + 2, row1, TEXT_COLOR_LIGHT_CYAN, TEXT_BG_BLACK);
    
    // Row 2 - QWERTY row
    const char* row2 = "Q W E R T Y U I O P";
    text_put_string(x + 2, y + 3, row2, TEXT_COLOR_LIGHT_CYAN, TEXT_BG_BLACK);
    
    // Row 3 - ASDFGH row
    const char* row3 = " A S D F G H J K L";
    text_put_string(x + 2, y + 4, row3, TEXT_COLOR_LIGHT_CYAN, TEXT_BG_BLACK);
    
    // Row 4 - ZXCVBN row
    const char* row4 = "  Z X C V B N M";
    text_put_string(x + 2, y + 5, row4, TEXT_COLOR_LIGHT_CYAN, TEXT_BG_BLACK);
}

/*
 * Display US Dvorak keyboard layout
 */
static void display_dvorak_layout(uint8_t x, uint8_t y)
{
    // Row 1 - Number row (same as QWERTY)
    const char* row1 = "1 2 3 4 5 6 7 8 9 0";
    text_put_string(x + 2, y + 2, row1, TEXT_COLOR_LIGHT_CYAN, TEXT_BG_BLACK);
    
    // Row 2 - Dvorak top
    const char* row2 = "' , . P Y F G C R L";
    text_put_string(x + 2, y + 3, row2, TEXT_COLOR_LIGHT_CYAN, TEXT_BG_BLACK);
    
    // Row 3 - Dvorak middle
    const char* row3 = " A O E U I D H T N S";
    text_put_string(x + 2, y + 4, row3, TEXT_COLOR_LIGHT_CYAN, TEXT_BG_BLACK);
    
    // Row 4 - Dvorak bottom
    const char* row4 = "  ; Q J K X B M W V Z";
    text_put_string(x + 2, y + 5, row4, TEXT_COLOR_LIGHT_CYAN, TEXT_BG_BLACK);
}

/*
 * Display Colemak keyboard layout
 */
static void display_colemak_layout(uint8_t x, uint8_t y)
{
    // Row 1 - Number row (same)
    const char* row1 = "1 2 3 4 5 6 7 8 9 0";
    text_put_string(x + 2, y + 2, row1, TEXT_COLOR_LIGHT_CYAN, TEXT_BG_BLACK);
    
    // Row 2 - Colemak top
    const char* row2 = "Q W F P B J L U Y ;";
    text_put_string(x + 2, y + 3, row2, TEXT_COLOR_LIGHT_CYAN, TEXT_BG_BLACK);
    
    // Row 3 - Colemak middle
    const char* row3 = " A R S T D H N E I O";
    text_put_string(x + 2, y + 4, row3, TEXT_COLOR_LIGHT_CYAN, TEXT_BG_BLACK);
    
    // Row 4 - Colemak bottom
    const char* row4 = "  Z X C V K M , . /";
    text_put_string(x + 2, y + 5, row4, TEXT_COLOR_LIGHT_CYAN, TEXT_BG_BLACK);
}

/*
 * Display an active keyboard layout box in the center of the screen
 */
void display_keyboard_layout_box(keyboard_layout_manager_t* manager, 
                                 uint8_t x, uint8_t y, 
                                 uint8_t width, uint8_t height)
{
    // Draw the outer box
    text_draw_box(x, y, width, height, 
                  TEXT_COLOR_LIGHT_GREEN, TEXT_BG_BLACK, "KEYBOARD LAYOUT");
    
    // Get current layout name
    const char* layout_name = keyboard_layout_get_current_name(manager);
    
    // Display layout name and index
    char layout_info[40];
    for (int i = 0; i < string_length(layout_name); i++) {
        layout_info[i] = layout_name[i];
    }
    layout_info[string_length(layout_name)] = ' ';
    layout_info[string_length(layout_name) + 1] = '[';
    
    char num_str[4];
    int_to_string(manager->current_layout + 1, num_str, 4);
    int pos = string_length(layout_name) + 2;
    for (int i = 0; num_str[i] != '\0'; i++) {
        layout_info[pos + i] = num_str[i];
    }
    pos += string_length(num_str);
    layout_info[pos] = '/';
    pos++;
    int_to_string(LAYOUT_COUNT, num_str, 4);
    for (int i = 0; num_str[i] != '\0'; i++) {
        layout_info[pos + i] = num_str[i];
    }
    pos += string_length(num_str);
    layout_info[pos] = ']';
    layout_info[pos + 1] = '\0';
    
    text_put_string(x + 2, y + 1, layout_info, TEXT_COLOR_YELLOW, TEXT_BG_BLACK);
    
    // Display keyboard layout based on current layout type
    switch (manager->current_layout) {
        case LAYOUT_US_DVORAK:
            display_dvorak_layout(x, y);
            break;
        case LAYOUT_COLEMAK:
            display_colemak_layout(x, y);
            break;
        case LAYOUT_US_QWERTY:
        default:
            display_qwerty_layout(x, y);
            break;
    }
}

/*
 * Display a centered keyboard layout box
 */
void display_keyboard_layout_centered(keyboard_layout_manager_t* manager)
{
    // Calculate center position for a 60x10 box in 80x25 terminal
    uint8_t box_width = 60;
    uint8_t box_height = 9;
    uint8_t box_x = (80 - box_width) / 2;
    uint8_t box_y = (25 - box_height) / 2;
    
    display_keyboard_layout_box(manager, box_x, box_y, box_width, box_height);
}

