#include "ssd1306.h"

// --- Macros -- 
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

void driver_i2c_init() {
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void display_init(ssd1306_t *disp) {
    driver_i2c_init();

    disp->external_vcc=false;
    ssd1306_init(disp, 128, 64, 0x3C, I2C_PORT);
    ssd1306_clear(disp);
}