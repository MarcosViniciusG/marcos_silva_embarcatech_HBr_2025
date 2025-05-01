#include <stdio.h>
#include "temperature.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define ADC_TEMPERATURE_CHANNEL 4

void initADC() {
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(ADC_TEMPERATURE_CHANNEL);
}

int main()
{
    initADC();

    while (true) {
        printf("%d\n", adc_to_celsius(4096));
        sleep_ms(1000);
    }

    return 0;
}
