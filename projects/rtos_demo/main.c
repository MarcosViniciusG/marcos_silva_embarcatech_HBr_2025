#include <stdio.h>
#include "init_hardware.h"
#include "FreeRTOS.h"
#include "task.h"

// --- Global Variables ---
static uint8_t current_led = RED_LED_PIN;
static TaskHandle_t blink_task_handle = NULL;
static TaskHandle_t buzzer_task_handle = NULL;

// --- Prototypes ---
void set_led(bool r, bool g, bool b);
void blink_task(void *params);
void buzzer_task(void *params); 
void button_task(void *params); 

int main() {
    stdio_init_all();
    init_hardware();

    xTaskCreate(blink_task, "Blink", 256, NULL, 1, &blink_task_handle);
    xTaskCreate(buzzer_task, "Buzzer", 256, NULL, 1, &buzzer_task_handle);
    xTaskCreate(button_task, "Button", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true) {}
    return 0;
}

void set_led(bool r, bool g, bool b) 
{
    gpio_put(RED_LED_PIN, r);
    gpio_put(GREEN_LED_PIN, g);
    gpio_put(BLUE_LED_PIN, b);
}

void blink_task(void *params) 
{
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

void buzzer_task(void *params) 
{
    while (true) {
        gpio_put(BUZZER_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(50)); // Beep for 50 ms
        gpio_put(BUZZER_PIN, 0); 
        vTaskDelay(pdMS_TO_TICKS(950));
    }
}

void button_task(void *params) 
{
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