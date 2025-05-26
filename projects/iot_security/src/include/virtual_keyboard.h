#ifndef _VIRTUAL_KEYBOARD_H_
#define _VIRTUAL_KEYBOARD_H_

#include "pico/stdlib.h"
#include "ssd1306.h" // Include the SSD1306 display driver header

// --- Configuration Defines  ---

// Text input buffer size
#ifndef MAX_TEXT_LENGTH
#define MAX_TEXT_LENGTH 63
#endif

// --- Public Functions ---

/**
 * @brief Draws the virtual keyboard layout and the current input text on the OLED display.
 * @param disp is the display address
 * @param anonymous tells if the text should be hidden 
 */
void virtual_keyboard_draw(ssd1306_t *disp, bool anonymous);

/**
 * @brief Handles input from the joystick and buttons to navigate the keyboard and select characters.
 * @return True if the virtual has finished, false otherwise.
 */
bool virtual_keyboard_handle_input();

/**
 * @brief Gets the current text string entered by the user.
 * @return A constant pointer to the null-terminated string containing the current input text.
 */
const char* virtual_keyboard_get_text();

/**
 * @brief Clears the current text string in the input buffer.
 */
void virtual_keyboard_clear_text();

#endif // _VIRTUAL_KEYBOARD_H_