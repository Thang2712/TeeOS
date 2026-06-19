# Keyboard Layout Display Feature

## Overview
This feature adds a Unix-style box display to show the user's active keyboard layout in the TeeOS kernel.

## Components Created

### 1. Keyboard Layout Manager (`include/drivers/keyboard_layout.h`, `src/drivers/keyboard_layout.c`)
- **Purpose**: Manages keyboard layout state and switching
- **Features**:
  - Supports multiple layout types (US QWERTY, US Dvorak, Colemak)
  - Initialize with default US QWERTY layout
  - Functions to switch between layouts (next, previous, set specific)
  - Get current layout name

### 2. Text Box Drawing Utilities (`include/common/text_box.h`, `src/common/text_box.c`)
- **Purpose**: Provides text-mode UI utilities using VGA text mode at 0xB8000
- **Features**:
  - Character and string output at specific screen positions
  - Box drawing with Unicode box characters (IBM PC extended ASCII)
  - Support for foreground/background colors
  - Horizontal and vertical line drawing
  - Full bordered box drawing with optional title support
  - Cursor movement

### 3. Keyboard Layout Display (`include/drivers/keyboard_layout_display.h`, `src/drivers/keyboard_layout_display.c`)
- **Purpose**: Display keyboard layout information in a formatted box
- **Features**:
  - Display layout box at custom position
  - Centered display helper function
  - Shows current layout with index (e.g., "1/3")
  - Color-coded display with navigation hints

### 4. Kernel Integration (`src/kernel.c`)
- **Changes**:
  - Added `layout` command to kernel CLI
  - Initialized keyboard layout manager in `kernelMain()`
  - Display box shows active layout name and navigation info

## Usage

### Command Line
Type the following command in the TeeOS CLI:
```
layout
```

This will display a centered box showing:
- Current keyboard layout name
- Layout index (e.g., "1/3" for layout 1 of 3)
- Navigation hint: "Alt+Tab to switch layouts"

### Example Output
```
┌──────────────────────────────────────┐
│      KEYBOARD LAYOUT                 │
│                                      │
│           US QWERTY                  │
│                                      │
│             Layout: 1/3              │
│                                      │
│ Alt+Tab to switch layouts            │
└──────────────────────────────────────┘
```

## Box Drawing Characters
The implementation uses IBM PC extended ASCII box drawing characters:
- `┌` (218): Top-left corner
- `┐` (191): Top-right corner
- `└` (192): Bottom-left corner
- `┘` (217): Bottom-right corner
- `─` (196): Horizontal line
- `│` (179): Vertical line
- And additional corner/junction characters for more complex boxes

## Color Support
Supported colors (16 colors in VGA text mode):
- Black, Blue, Green, Cyan, Red, Magenta, Brown, Light Grey
- Dark Grey, Light Blue, Light Green, Light Cyan, Light Red, Light Magenta, Yellow, White

## API Functions

### Keyboard Layout Manager
```c
void keyboard_layout_manager_init(keyboard_layout_manager_t* manager);
const char* keyboard_layout_get_current_name(keyboard_layout_manager_t* manager);
void keyboard_layout_next(keyboard_layout_manager_t* manager);
void keyboard_layout_previous(keyboard_layout_manager_t* manager);
void keyboard_layout_set(keyboard_layout_manager_t* manager, keyboard_layout_type layout);
```

### Text Box Drawing
```c
void text_put_char(uint8_t x, uint8_t y, uint8_t ch, text_color_t fg, text_bg_t bg);
void text_put_string(uint8_t x, uint8_t y, const char* str, text_color_t fg, text_bg_t bg);
void text_draw_box(uint8_t x, uint8_t y, uint8_t width, uint8_t height, 
                   text_color_t fg, text_bg_t bg, const char* title);
void text_move_cursor(uint8_t x, uint8_t y);
```

### Layout Display
```c
void display_keyboard_layout_box(keyboard_layout_manager_t* manager, 
                                 uint8_t x, uint8_t y, 
                                 uint8_t width, uint8_t height);
void display_keyboard_layout_centered(keyboard_layout_manager_t* manager);
```

## Future Enhancements
- Add Alt+Tab keyboard shortcut for layout switching
- Persist layout selection
- Add more keyboard layout types (DVORAK, COLEMAK actual key mappings)
- Extended layout configuration system
- Layout-specific character mapping for keyboard driver

## Files Modified
- `CMakeLists.txt` - Updated with new source files
- `makefile` - Updated object file list
- `src/kernel.c` - Added initialization and `layout` command

## Files Created
- `include/drivers/keyboard_layout.h`
- `src/drivers/keyboard_layout.c`
- `include/drivers/keyboard_layout_display.h`
- `src/drivers/keyboard_layout_display.c`
- `include/common/text_box.h`
- `src/common/text_box.c`
