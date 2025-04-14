#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include <stdio.h> 

// Macros
#define BTN_A 5
#define BTN_B 6

#define I2C_SDA 14
#define I2C_SCL 15
typedef struct render_area render_area;

// Global variables
volatile bool start = false;
volatile bool update = false;
volatile bool countdownStarted = false;
volatile uint8_t counter=0;
volatile uint8_t clickCounter=0;
volatile absolute_time_t lastClickTime=0;

render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};
uint8_t ssd[ssd1306_buffer_length];

// Prototypes
void initDisplay();
void initButtons();
void irqCallback();
void startCountdown();
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

// Buttons initialization
void initButtons()
{
    gpio_init(BTN_A);
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_pull_up(BTN_A);
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_FALL, true, &irqCallback);

    gpio_init(BTN_B);
    gpio_set_dir(BTN_B, GPIO_IN);
    gpio_pull_up(BTN_B);
    gpio_set_irq_enabled(BTN_B, GPIO_IRQ_EDGE_FALL, true);
}

// Interrupt callback function
void irqCallback(uint gpio, uint32_t events) {
    if(gpio==BTN_A && !countdownStarted)
        start = true;
    else if(gpio==BTN_B && countdownStarted && absolute_time_diff_us(lastClickTime, get_absolute_time()) > 200000) {
        clickCounter++;
        update = true;
        lastClickTime = get_absolute_time();
    }
}

void startCountdown() {
    countdownStarted = true;
    clickCounter=0;
    counter=9;
    while(counter > 0) {
        updateDisplay();
        sleep_ms(1000);
        counter--;
    }

    countdownStarted = false;
    updateDisplay();
    sleep_ms(3000);
}

void updateDisplay() {
    // Clear the display
    memset(ssd, 0, sizeof(ssd));
    render_on_display(ssd, &frame_area);

    // Draw countdown timer
    ssd1306_draw_string(ssd, 5, 0, "Countdown:");
    int len = snprintf(NULL, 0, "%d", counter);
    char *result = malloc(len + 1);
    snprintf(result, len + 1, "%d", counter);  
    ssd1306_draw_string(ssd, 5, 8, result);

    // Draw clicks counter
    ssd1306_draw_string(ssd, 5, 24, "Clicks:");
    len = snprintf(NULL, 0, "%d", clickCounter);
    result = malloc(len + 1);
    snprintf(result, len + 1, "%d", clickCounter);  
    ssd1306_draw_string(ssd, 5, 32, result);

    render_on_display(ssd, &frame_area);
}

int main()
{
    // Initialization
    stdio_init_all();
    initDisplay(&frame_area);
    initButtons();

    bool countdownStarted = false;
    while (true) {
        if(start) {
            start=false;
            startCountdown();
        }
        if(update) {
            update=false;
            updateDisplay();
        }
    }

    return 0;
}
