#ifndef _INIT_HARDWARE_H_
#define _INIT_HARDWARE_H_

#include "pico/stdlib.h"
#include "ssd1306.h"

// ADC conversion factor
#define ADC_CONVERSION_FACTOR (3.3f / (1 << 12))

// Joystick movement thresholds
#define JOYSTICK_THRESHOLD_HIGH 3000
#define JOYSTICK_THRESHOLD_LOW 1000

// OLED display dimensions
#ifndef OLED_WIDTH
#define OLED_WIDTH 128
#endif
#ifndef OLED_HEIGHT
#define OLED_HEIGHT 64
#endif

// I2C defines for the OLED display
#ifndef I2C_PORT
#define I2C_PORT i2c1 // Default to i2c1
#endif
#ifndef I2C_SDA_PIN
#define I2C_SDA_PIN 14 // Default SDA pin for i2c1
#endif
#ifndef I2C_SCL_PIN
#define I2C_SCL_PIN 15 // Default SCL pin for i2c1
#endif
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR 0x3C // Default I2C address for SSD1306
#endif

// Button GPIOs
#ifndef BUTTON_A_PIN
#define BUTTON_A_PIN 5
#endif
#ifndef BUTTON_B_PIN
#define BUTTON_B_PIN 6
#endif
#ifndef JOYSTICK_SW_PIN
#define JOYSTICK_SW_PIN 22
#endif

// Joystick ADC GPIOs
#ifndef JOYSTICK_VRX_PIN
#define JOYSTICK_VRX_PIN 27 // Connected to GP27 (ADC1)
#endif
#ifndef JOYSTICK_VRY_PIN
#define JOYSTICK_VRY_PIN 26 // Connected to GP26 (ADC0)
#endif

/**
 * @brief Initializes the hardware.
 * This includes setting up I2C for the display, GPIOs for buttons, and ADC for the joystick.
 * It also initializes the SSD1306 display driver.
 *
 * @param disp A pointer to the ssd1306_t structure used by the display driver.
 * @return true if initialization is successful, false otherwise.
 */
bool init_hardware(ssd1306_t *disp);

#endif // _INIT_HARDWARE_H_