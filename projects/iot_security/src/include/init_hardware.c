#include "init_hardware.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"

// Prototypes
static bool _init_display(ssd1306_t *disp);
static void _init_buttons();
static void _init_joystick();

bool init_hardware(ssd1306_t *disp) {
    bool success = _init_display(disp);
    _init_buttons();
    _init_joystick();
    return success;
}

// --- Private Helper Function Implementations ---

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

static void _init_joystick() {
    gpio_init(JOYSTICK_SW_PIN);
    gpio_set_dir(JOYSTICK_SW_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_SW_PIN);

    // ADC initialization for joystick
    adc_init();
    adc_gpio_init(JOYSTICK_VRY_PIN); // VRy
    adc_gpio_init(JOYSTICK_VRX_PIN); // VRx
}