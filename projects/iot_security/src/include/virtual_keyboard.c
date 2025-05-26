#include "virtual_keyboard.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "string.h"
#include "init_hardware.h"
#include "stdio.h"

// --- Internal Defines ---
#define KEYBOARD_ROWS 5
#define KEYBOARD_COLS 10

// Virtual Keyboard Layout (Simplified QWERTY-like)
// Using a 5x10 character grid for simplicity
const char keyboard_layout[5][10] = {
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
    {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
    {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':'},
    {'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '?'},
    {' ', ' ', '<', '>', '_', ' ', ' ', ' ', ' ', ' '} // Space, Backspace, Enter/Send
};

// --- Internal Global Variables ---
static int _cursor_x = 0;
static int _cursor_y = 0;

static char _input_text[MAX_TEXT_LENGTH + 1] = "";
static int _text_length = 0;

static uint32_t _last_action_time = 0; // For debouncing

// --- Private Helper Functions ---
static void _handle_character_input(char selected_char);

// --- Public Function Implementations ---

void virtual_keyboard_draw(ssd1306_t *disp, bool anonymous) {
    int char_width = 8;
    int char_height = 8; 
    int start_y = (OLED_HEIGHT / 2) - 8;

    // Clear the whole display to redraw everything (simpler for this example)
    ssd1306_clear(disp);

    // Draw the current input text if not anonymous,
    // else draw '*'
    if(!anonymous) 
        for(int i=0; i<strlen(_input_text); i++) 
            ssd1306_draw_char(disp, (i%21)*6, (i/21)*8, 1, _input_text[i]);
    else 
        for(int i=0; i<strlen(_input_text); i++) 
            ssd1306_draw_char(disp, (i%21)*6, (i/21)*8, 1, '*');

    // Draw separator line
    ssd1306_draw_line(disp, 0, start_y - 1, OLED_WIDTH - 1, start_y - 1);

    // Draw keyboard layout
    for (int y = 0; y < KEYBOARD_ROWS; ++y) {
        for (int x = 0; x < KEYBOARD_COLS; ++x) {
            int draw_x = x * char_width;
            int draw_y = start_y + y * char_height;

            // Highlight the selected key
            if (x == _cursor_x && y == _cursor_y) {
                // Invert colors for selected key
                ssd1306_draw_square(disp, draw_x, draw_y, char_width, char_height); // Draw a filled square
                ssd1306_draw_char(disp, draw_x, draw_y, 1, keyboard_layout[y][x]);
            } else {
                ssd1306_draw_char(disp, draw_x, draw_y, 1, keyboard_layout[y][x]);
            }
        }
    }
}

bool virtual_keyboard_handle_input() {
    // Debounce inputs
    if (to_ms_since_boot(get_absolute_time()) - _last_action_time < 200) {
        return false;
    }

    bool action_taken = false;

    // Read joystick analog values
    adc_select_input(0); // Select ADC0 (GP26) for VRy
    uint16_t vry_raw = adc_read();
    adc_select_input(1); // Select ADC1 (GP27) for VRx
    uint16_t vrx_raw = adc_read();

    // Handle joystick movement for cursor navigation
    if (vry_raw > JOYSTICK_THRESHOLD_HIGH) { // Up
        _cursor_y = (_cursor_y > 0) ? _cursor_y - 1 : KEYBOARD_ROWS - 1;
        action_taken = true;
    } else if (vry_raw < JOYSTICK_THRESHOLD_LOW) { // Down
        _cursor_y = (_cursor_y < KEYBOARD_ROWS - 1) ? _cursor_y + 1 : 0;
        action_taken = true;
    }

    if (vrx_raw > JOYSTICK_THRESHOLD_HIGH) { // Right
        _cursor_x = (_cursor_x < KEYBOARD_COLS - 1) ? _cursor_x + 1 : 0;
        action_taken = true;
    } else if (vrx_raw < JOYSTICK_THRESHOLD_LOW) { // Left
        _cursor_x = (_cursor_x > 0) ? _cursor_x - 1 : KEYBOARD_COLS - 1;
        action_taken = true;
    }

    // Read button states
    bool button_a_pressed = !gpio_get(BUTTON_A_PIN);
    bool button_b_pressed = !gpio_get(BUTTON_B_PIN);
    bool joystick_sw_pressed = !gpio_get(JOYSTICK_SW_PIN);

    // Handle character selection via joystick switch or Button A
    if (joystick_sw_pressed || button_a_pressed) {
        char selected_char = keyboard_layout[_cursor_y][_cursor_x];
        if(selected_char=='>')
            return true;
            
        _handle_character_input(keyboard_layout[_cursor_y][_cursor_x]);
        action_taken = true;
    }

    // Handle Button B
    if (button_b_pressed) {
        action_taken = true;
    }

    if (action_taken) {
        _last_action_time = to_ms_since_boot(get_absolute_time());
    }

    return false;
}

const char* virtual_keyboard_get_text() {
    return _input_text;
}

void virtual_keyboard_clear_text() {
    _text_length = 0;
    _input_text[0] = '\0';
    _cursor_x = 0;
    _cursor_y = 0;
}

// --- Private Helper Function Implementation ---

static void _handle_character_input(char selected_char) {
    if (selected_char == '<') { // Backspace
        if (_text_length > 0) {
            _input_text[--_text_length] = '\0';
        }
    }  
    else { // Regular character
        if (_text_length < MAX_TEXT_LENGTH) {
            _input_text[_text_length++] = selected_char;
            _input_text[_text_length] = '\0';
        }
    }
}