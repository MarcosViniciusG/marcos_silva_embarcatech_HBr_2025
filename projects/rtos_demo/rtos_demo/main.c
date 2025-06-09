#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#define RGB_LED_G 12

void blink_task(void *params) {
    const uint LED_PIN = RGB_LED_G;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main() {
    stdio_init_all();
    xTaskCreate(blink_task, "Blink", 256, NULL, 1, NULL);
    vTaskStartScheduler();
    while (true) {}
}