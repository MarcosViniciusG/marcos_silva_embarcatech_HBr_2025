#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/irq.h"
#include "ssd1306.h"

// Button GPIOs
#ifndef BUTTON_A_PIN
#define BUTTON_A_PIN 5
#endif
#ifndef BUTTON_B_PIN
#define BUTTON_B_PIN 6
#endif

#ifndef RED_LED_PIN 
#define RED_LED_PIN 13
#endif
#ifndef GREEN_LED_PIN
#define GREEN_LED_PIN 11
#endif

// OLED display dimensions
#ifndef OLED_WIDTH
#define OLED_WIDTH 128
#endif
#ifndef OLED_HEIGHT
#define OLED_HEIGHT 64
#endif

// I2C defines for the OLED display
#ifndef I2C_PORT
#define I2C_PORT i2c1 // Default to i2c1
#endif
#ifndef I2C_SDA_PIN
#define I2C_SDA_PIN 14 // Default SDA pin for i2c1
#endif
#ifndef I2C_SCL_PIN
#define I2C_SCL_PIN 15 // Default SCL pin for i2c1
#endif
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR 0x3C // Default I2C address for SSD1306
#endif

#ifndef MIC_CHANNEL
#define MIC_CHANNEL 2
#endif

#ifndef MIC_PIN
#define MIC_PIN (26 + MIC_CHANNEL)
#endif

typedef enum
{
    STATE_NOTHING,
    STATE_RECORDING,
    STATE_PLAYING
} state_t;

static state_t global_state = STATE_NOTHING;

static bool _init_display(ssd1306_t *disp);
static void _init_buttons();
static void _init_leds();
void gpio_callback(uint gpio, uint32_t events);
void display_samples(ssd1306_t *disp, short *samples_buffer, uint8_t samples_num);
void sample_mic(short *samples_buffer, uint8_t samples_num);

signed main() {
    stdio_init_all();

    adc_init();
    adc_gpio_init(28);
    adc_select_input(2);
    _init_buttons();
    _init_leds();
    ssd1306_t disp;
    if(!_init_display(&disp)) {
        printf("Failed to init display.Exiting program\n");
        return 0;
    }

    gpio_put(RED_LED_PIN, 0);
    gpio_put(GREEN_LED_PIN, 0);

    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);

    short samples_buffer[128];
    while (1) {
        if(global_state==STATE_RECORDING) {
            sample_mic(samples_buffer, 128);
            display_samples(&disp, samples_buffer, 128);
            sleep_ms(10);
        }
        else if(global_state==STATE_PLAYING) {

        }
        sleep_ms(10);
    }
}

static bool _init_display(ssd1306_t *disp) {
    i2c_init(I2C_PORT, 400 * 1000); // 400 kHz for I2C communication
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    disp->external_vcc = false; // Internal VCC
    bool success = ssd1306_init(disp, OLED_WIDTH, OLED_HEIGHT, SSD1306_I2C_ADDR, I2C_PORT);
    
    return success;
}

static void _init_leds() {
    gpio_init(RED_LED_PIN);
    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
}

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A_PIN && global_state != STATE_RECORDING) {
        global_state = STATE_RECORDING;
        gpio_put(RED_LED_PIN, 1);
        gpio_put(GREEN_LED_PIN, 0);
    } else if (gpio == BUTTON_B_PIN && global_state != STATE_PLAYING) {
        global_state = STATE_PLAYING;
        gpio_put(RED_LED_PIN, 0);
        gpio_put(GREEN_LED_PIN, 1);
    }
}

static void _init_buttons() {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
}

void display_samples(ssd1306_t *disp, short *samples_buffer, uint8_t samples_num) {
    ssd1306_clear(disp);
    for(uint8_t x=0; x<samples_num; x++) {
        short h = 32 - (samples_buffer[x] / 66);
        if(h >= 32)
            ssd1306_draw_line(disp, x, 32, x, h);  
        else
             ssd1306_draw_line(disp, x, h, x, 32);  

    }

    ssd1306_show(disp);
}

void sample_mic(short *samples_buffer, uint8_t samples_num) {
    for(uint8_t i=0; i<samples_num; i++) {
        uint16_t res = adc_read(); 
        printf("%d\n", res);
        samples_buffer[i] = res - 2048;
    }
}