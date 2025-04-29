#include "hal_led.h"
#include "pico/stdlib.h"

int main(void) {
    while (true) {
        hal_led_toggle();
        sleep_ms(500);
    }

    return 0;
}