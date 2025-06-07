#include "init_hardware.h"

bool init_hardware(ssd1306_t *disp) {
    bool success = _init_display(disp);
    if(success) {
        _init_buttons();
        _init_leds();
        _init_mic();
        _init_audio();
    }

    return success;
}

// --- Private Helper Functions ---
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
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
}

static void _init_mic() {
    adc_init();
    adc_gpio_init(28);
    adc_select_input(2);
}

static void _init_audio() {
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint channel_num = pwm_gpio_to_channel(BUZZER_PIN);

    pwm_set_wrap(slice_num, 255);
    pwm_set_clkdiv(slice_num, 1.0f); 
    pwm_set_chan_level(slice_num, channel_num, 0); 
    pwm_set_enabled(slice_num, true);
}
