#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "init_hardware.h"

// --- Macros ---
#define SAMPLE_RATE 8000                        // In samples/second
#define SAMPLE_TIME 5                            // In seconds
#define TOTAL_SAMPLES SAMPLE_RATE * SAMPLE_TIME

typedef enum
{
    STATE_NOTHING,
    STATE_RECORDING,
    STATE_PLAYING
} state_t;

// --- Global variables ---
static state_t global_state = STATE_NOTHING;

// --- Prototypes ---
void gpio_callback(uint gpio, uint32_t events);
void display_waveform(ssd1306_t *disp, uint8_t *buffer, uint32_t start, uint32_t end);
void record_audio(uint8_t *buffer, uint32_t max_size, uint32_t *current_idx, uint32_t sample_rate, ssd1306_t *disp);
void play_audio(uint8_t *buffer, uint32_t num_samples, uint32_t sample_rate, ssd1306_t *disp);
void draw_start(ssd1306_t *disp);

signed main() {
    stdio_init_all();
    ssd1306_t disp;
    if(!init_hardware(&disp)) {
        printf("Failed to init display.Exiting program\n");
        return 0;
    }

    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);

    uint8_t buffer[TOTAL_SAMPLES];
    uint32_t current_idx = 0;

    while (1) {
        if(global_state==STATE_RECORDING) {
            record_audio(buffer, TOTAL_SAMPLES, &current_idx, SAMPLE_RATE, &disp);
            global_state = STATE_NOTHING;
        }
        else if(global_state==STATE_PLAYING) {
            play_audio(buffer, current_idx, SAMPLE_RATE, &disp);
            global_state = STATE_NOTHING;
        }
        else {
            draw_start(&disp);
            gpio_put(RED_LED_PIN, 0);
            gpio_put(GREEN_LED_PIN, 0);
        }
        sleep_ms(10);
    }
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

void display_waveform(ssd1306_t *disp, uint8_t *buffer, uint32_t start, uint32_t end) {
    ssd1306_clear(disp);
    for(uint8_t x=start; x<end; x++) {
        short h = 32 - (buffer[x] / 66);
        if(h >= 32)
            ssd1306_draw_line(disp, x-start, 32, x-start, h);  
        else
             ssd1306_draw_line(disp, x-start, h, x-start, 32);  

    }

    ssd1306_show(disp);
}

void record_audio(uint8_t *buffer, uint32_t max_size, uint32_t *current_idx, uint32_t sample_rate, ssd1306_t *disp) {
    uint32_t delay_us = 1000000 / sample_rate;

    *current_idx = 0;

    uint64_t start_time_us = time_us_64();

    while (*current_idx < max_size) {
        uint16_t raw_adc = adc_read();
        int32_t processed_adc = raw_adc - 2048;
        processed_adc = processed_adc / 16;
        processed_adc += 128;

        if (processed_adc < 0) processed_adc = 0;
        if (processed_adc > 255) processed_adc = 255;

        buffer[*current_idx] = (uint8_t)processed_adc;
        (*current_idx)++;

        uint64_t elapsed_us = time_us_64() - start_time_us;
        uint64_t target_time_us = (*current_idx) * delay_us;
        if (elapsed_us < target_time_us) {
            sleep_us(target_time_us - elapsed_us);
        }
    }
}

void play_audio(uint8_t *buffer, uint32_t num_samples, uint32_t sample_rate, ssd1306_t *disp) {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint channel_num = pwm_gpio_to_channel(BUZZER_PIN);

    uint32_t delay_us = 1000000 / sample_rate;

    uint64_t start_time_us = time_us_64();
    for (uint32_t i = 0; i < num_samples; ++i) {
        pwm_set_chan_level(slice_num, channel_num, buffer[i]);

        uint64_t elapsed_us = time_us_64() - start_time_us;
        uint64_t target_time_us = (i + 1) * delay_us;
        if (elapsed_us < target_time_us) {
            sleep_us(target_time_us - elapsed_us);
        }

        int limit = (i >= OLED_WIDTH) ? i - OLED_WIDTH : 0;
        display_waveform(disp, buffer, limit, i);
    }
    pwm_set_chan_level(slice_num, channel_num, 0);
}

void draw_start(ssd1306_t *disp) {
    ssd1306_clear(disp);
    ssd1306_draw_string(disp, 20, 28, 1, "(A) Gravar");
    ssd1306_draw_string(disp, 20, 38, 1, "(B) Reproduzir");
    ssd1306_show(disp);
}