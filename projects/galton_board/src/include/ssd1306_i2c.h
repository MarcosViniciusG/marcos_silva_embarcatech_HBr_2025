#include "hardware/i2c.h"
#include "ssd1306.h"

/*
    Initializes I2C 
*/
void driver_i2c_init();

/*
    Initializes display
    (init and clear)
*/
void display_init(ssd1306_t *disp);