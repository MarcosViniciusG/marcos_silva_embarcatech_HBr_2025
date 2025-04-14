#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include <stdio.h> 

// Macros
#define I2C_SDA 14
#define I2C_SCL 15
#define VRX 27
#define VRY 26
#define SW 22
typedef struct render_area render_area;

// Global variables
volatile uint16_t valx;
volatile uint16_t valy;
render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};
uint8_t ssd[ssd1306_buffer_length];

// Prototypes
void initDisplay();
void initJoystick();
void updateDisplay();

// Display initialization
void initDisplay()
{
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();
    calculate_render_area_buffer_length(&frame_area);
}

// Joystick initialization
void initJoystick() {
    adc_init();
    adc_gpio_init(VRX);
    adc_gpio_init(VRY);

    gpio_init(SW);
    gpio_set_dir(SW, GPIO_IN);
    gpio_pull_up(SW);
}

void updateDisplay() {
    // Clear the display
    memset(ssd, 0, sizeof(ssd));
    render_on_display(ssd, &frame_area);

    // Draw joystick ADC readings
    char x[16];
    sprintf(x, "X:  %d", valx);
    ssd1306_draw_string(ssd, 5, 24, x);

    char y[16];
    sprintf(y, "Y:  %d", valy);
    ssd1306_draw_string(ssd, 5, 32, y);

    render_on_display(ssd, &frame_area);
}

int main()
{
    // Initialization
    stdio_init_all();
    initDisplay();
    initJoystick();

    bool countdownStarted = false;
    while (true) {
        adc_select_input(0);
        valy = adc_read();

        adc_select_input(1);
        valx = adc_read();

        updateDisplay();
        sleep_ms(1000);
    }

    return 0;
}
