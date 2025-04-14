#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include <stdio.h> 

// Macros
#define I2C_SDA 14
#define I2C_SCL 15
#define ADC_TEMPERATURE_CHANNEL 4
typedef struct render_area render_area;

render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};
uint8_t ssd[ssd1306_buffer_length];

// Prototypes
void initDisplay();
void initADC();
float ADCToTemperature(uint16_t adcValue);
void updateDisplay(float temperature);

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

// ADC initialization
void initADC() {
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(ADC_TEMPERATURE_CHANNEL);
}

float ADCToTemperature(uint16_t adcValue)
{
  const float conversion = 3.3f / (1 << 12);
  float voltage = adcValue * conversion;
  float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
  return temperature;
}

void updateDisplay(float temperature) {
    // Clear the display
    memset(ssd, 0, sizeof(ssd));
    render_on_display(ssd, &frame_area);

    // Draw temperature
    ssd1306_draw_string(ssd, 5, 24, "Temperature:");
    int len = snprintf(NULL, 0, "%.2f", temperature);
    char *result = malloc(len + 1);
    snprintf(result, len + 1, "%.2f", temperature);  
    ssd1306_draw_string(ssd, 5, 32, result);
    ssd1306_draw_string(ssd, 53, 32, "C");

    render_on_display(ssd, &frame_area);
}

int main()
{
    // Initialization
    stdio_init_all();
    initDisplay();
    initADC();

    bool countdownStarted = false;
    while (true) {
        updateDisplay(ADCToTemperature(adc_read()));
        sleep_ms(3000);
    }

    return 0;
}
