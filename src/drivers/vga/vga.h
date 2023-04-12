#ifndef VGA_H
#define VGA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// VGA text driver //

#define VGA_ROW 25
#define VGA_COL 80
/// VGA_ROW * VGA_COL
#define VGA_SCREEN_SIZE 2000

/// max number of screens
#define VGA_SCREEN_MAX 12

/// default character used when wrapping
#define VGA_WRAP_DEFAULT_CHAR '>'

/// default tab alignment
#define VGA_TAB_ALIGNMENT 4

#define VGA_CURSOR_HEIGHT 13, 15

/// HW address of the VGA buffer
#define VGA_HW_ADDR 0xB8000

/// CRTC I/O index addr, used to select reg
#define VGA_CRTC_INDEX 0x3D4
/// CRTC I/O data addr
#define VGA_CRTC_DATA 0x3D5

///  7 | CD, set == cursor disabled 5 | Cursor scan line start 4 3 2 1 0 |
#define VGA_CRTC_REG_CURSOR_START 0x0A
///  7 | CSK 6 5 | Cursor scan line end 4 3 2 1 0 |
#define VGA_CRTC_REG_CURSOR_END 0x0B
/// HIGH (<< 8) bytes of the VGA cursor location
#define VGA_CRTC_REG_CURSOR_LOCATION_HIGH 0x0E
/// LOW bytes of the VGA cursor location
#define VGA_CRTC_REG_CURSOR_LOCATION_LOW 0x0F

/// @brief VGA colors
enum vga_colors {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
};
/// @brief Extends the VGA background colors
enum vga_colors_fg {
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15,
};

typedef struct __attribute__((packed)) vga_attributes {
  enum vga_colors_fg fg : 4; /// character color
  enum vga_colors bg : 3;    /// background color (goes only up to 8)
  bool blink : 1;            /// blink (INOP?)
} vga_attributes;

/// @brief VGA text character
typedef struct __attribute__((packed)) vga_char {
  unsigned char c : 8;         /// CP437 character to print
  struct vga_attributes color; /// character color attributes
} vga_char;

/// @brief print control for vga print function.
/// Just partially initialize with what you actually need
typedef struct vga_info {
  uint8_t screen;         /// screen buffer to print to
  uint8_t column;         /// start at, max 79 (starts at 0)
  uint8_t row;            /// start at, max 24 (starts at 0)
  bool setcursor;         /// sets the cursor to column and row
                          /// neededed if row == 0 || col == 0
  bool nocursor;          /// append without loading or change the cursor state
  bool nowrap;            /// stop printing if the column overflows
  bool nowrapchar;        /// avoid printing a wrap character
  bool noscroll;          /// stop printing if the row overflows
  bool scrollattributes;  /// carry attributes over when scrolling
  bool noattributes;      /// don't load color info from the screen state
  bool print;             /// flush the screen buffer to VGA
  unsigned char wrapchar; /// the character to put when wrapping
  struct {
    bool cp437_print;   /// !INTERNAL! instead of using control chars print them
    bool cursor_loaded; /// !INTERNAL! is the screen cursor loaded?
  } internal; /// DO NOT TOUCH, sad sad sad lack of separation of concern
} vga_info;

// Functions //

// driver init

/// @brief initializes the vga driver, call before any other vga function
/// @param history_size needs to be at least 1, amount of screens worth of
/// history, N * 80*25 * sizeof(vga_char).
/// @param buffer_addr address of the screen buffers in memory
/// @return false if the screen doesn't exist or is already initalized
bool vga_init(size_t history_size, uint16_t *buffer_addr);

/// @brief output formatted string to a vga screen buffer (not posix)
/// @param info set the control info, pass a vga_info struct
/// @param format printf-like format string TODO document supported ops
/// @param ... arguments for printf format strings
/// @return negative if error, number of chars written otherwise
int vga_printf(vga_info info, const char *format, ...);

// getters/setters

/// @brief get the position of the cursor into column and row
/// @param screen_nbr screen to get the cursor position of
/// @param column destination variable for the column position
/// @param row destionation variable for the row position
/// @return negative if screen doesn't exist
bool vga_screen_getcursorpos(uint8_t screen_nbr, uint8_t *column, uint8_t *row);

/// @brief set the position of the cursor
/// @param screen_nbr screen to set the cursor position in
/// @param column cursor column position (starts at 0)
/// @param row cursor row position (starts at 0)
/// @return negative if screen doesn't exist or values overflow
bool vga_screen_setcursorpos(uint8_t screen_nbr, uint8_t column, uint8_t row);

/// @brief enable or disable the vga cursor attribute on a screen
/// @param screen_nbr screen to enable the cursor on
/// @param enable_cursor enable or disable cursor
/// @return negative if screen doesn't exist
bool vga_screen_setvgacursor(uint8_t screen_nbr, bool enable_cursor);

/// @brief set color attributes for next characters on a given screen
/// @param screen_nbr nbr of the screen to change attrs on
/// @param attributes color attributes to set
/// @return false if screen doesn't exist
bool vga_screen_setattributes(uint8_t screen_nbr, vga_attributes attributes);

/// @brief replace character color attributes for a given screen and set them
/// @param screen_nbr nbr of the screen to edit
/// @param attributes color attributes to override existing values with
/// @return false if screen doesn't exist
bool vga_screen_fillattributes(uint8_t screen_nbr, vga_attributes attributes);

/// @brief replace background color for a given screen and set it
/// @param screen_nbr nbr of the screen to edit
/// @param background_color background color to fill the screen with
/// @return false if screen doesn't exist
bool vga_screen_fillbackground(uint8_t screen_nbr,
                               enum vga_colors background_color);

/// @brief Get position in history buffer in screen
/// @param screen_nbr nbr of the screen to get history pos of
/// @return negative if screen doesn't exist, nbr or rows scrolled up in buffer
int vga_screen_getscrolloffset(uint8_t screen_nbr);

// display

/// @brief display the selected screen buffer
/// @param screen_nbr screen to print
/// @return false if screen doesn't exist
bool vga_screen_show(uint8_t screen_nbr);

/// @brief display the selected screen buffer, scrolled
/// @param screen_nbr screen to print
/// @param rows number of rows to scroll up by, -1 to go at the top
/// TODO @param scroll_overlay display position in buffer
/// @return negative if screen doesn't exist, 0 if at buffer head
int vga_screen_show_scrolled(uint8_t screen_nbr, int rows);

// reset

/// @brief clear the screen and reset cursor info
/// @param screen_nbr screen to clear
/// @return false if screen doesn't exist
bool vga_screen_clear(uint8_t screen_nbr);

// CRTC

/// @brief set the CD bit in the crtc
void vga_crtc_disable_cursor();

/// @brief enable the cursor and set it's scanlines (shape)
/// @param scanline_start needs to be < 15
/// @param scanline_end needs to be < 15
void vga_crtc_enable_cursor(uint8_t scanline_start, uint8_t scanline_end);

/// @brief enable the VGA cursor and set its position
/// @param column cursor column, starts at 0 must be < 80
/// @param row cursor row starts at 0 must be < 25
void vga_crtc_set_cursor(uint8_t column, uint8_t row);

#endif
