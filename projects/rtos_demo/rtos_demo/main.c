#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

// --- Macros ---
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

#define RED_LED_PIN 13
#define BLUE_LED_PIN 12
#define GREEN_LED_PIN 11

#define BUZZER_PIN 21

static uint8_t current_led = RED_LED_PIN;
static TaskHandle_t blink_task_handle = NULL;
static TaskHandle_t buzzer_task_handle = NULL;

static void _init_buttons() {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
}

static void _init_leds() {
    gpio_init(RED_LED_PIN);
    gpio_init(GREEN_LED_PIN);
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);
}

static void _init_audio() {
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
}

void set_led(bool r, bool g, bool b) {
    gpio_put(RED_LED_PIN, r);
    gpio_put(GREEN_LED_PIN, g);
    gpio_put(BLUE_LED_PIN, b);
}

void blink_task(void *params) {
    while (1) {
        gpio_put(current_led, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        switch (current_led)
        {
            case RED_LED_PIN:
                set_led(false, false, false);
                current_led = GREEN_LED_PIN;
                break;
            case GREEN_LED_PIN: 
                set_led(false, false, false);
                current_led = BLUE_LED_PIN;
                break;
            case BLUE_LED_PIN:
                set_led(false, false, false);
                current_led = RED_LED_PIN;
                break;
        }
    }
}

void buzzer_task(void *params) {
    while (true) {
        gpio_put(BUZZER_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(50)); 
        gpio_put(BUZZER_PIN, 0); 
        vTaskDelay(pdMS_TO_TICKS(950));
    }
}

void button_task(void *pvParameters) {
    bool is_led_suspended = false;
    bool is_buzzer_suspended = false;

    while (true) {
        if(!gpio_get(BUTTON_A_PIN)) {
            if (is_led_suspended) {
                vTaskResume(blink_task_handle);
                is_led_suspended = false;
            } else {
                vTaskSuspend(blink_task_handle);
                is_led_suspended = true;
            }
            vTaskDelay(pdMS_TO_TICKS(200)); // Anti-debounce
        }

        if (!gpio_get(BUTTON_B_PIN)) {
            if (is_buzzer_suspended) {
                vTaskResume(buzzer_task_handle);
                is_buzzer_suspended = false;
            } else {
                vTaskSuspend(buzzer_task_handle);
                is_buzzer_suspended = true;
            }
            vTaskDelay(pdMS_TO_TICKS(200)); // Anti-debounce
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // Polling
    }
}

int main() {
    stdio_init_all();
    _init_audio();
    _init_buttons();
    _init_leds();
    xTaskCreate(blink_task, "Blink", 256, NULL, 1, &blink_task_handle);
    xTaskCreate(buzzer_task, "Buzzer", 256, NULL, 1, &buzzer_task_handle);
    xTaskCreate(button_task, "Button", 256, NULL, 1, NULL);
    vTaskStartScheduler();
    while (true) {}
}