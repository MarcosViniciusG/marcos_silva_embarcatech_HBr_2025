#ifndef _INIT_HARDWARE_H
#define _INIT_HARDWARE_H

#include "ssd1306.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

// Button GPIOs
#ifndef BUTTON_A_PIN
#define BUTTON_A_PIN 5
#endif
#ifndef BUTTON_B_PIN
#define BUTTON_B_PIN 6
#endif

#ifndef RED_LED_PIN 
#define RED_LED_PIN 13
#endif
#ifndef GREEN_LED_PIN
#define GREEN_LED_PIN 11
#endif

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

#ifndef MIC_CHANNEL
#define MIC_CHANNEL 2
#endif

#ifndef MIC_PIN
#define MIC_PIN (26 + MIC_CHANNEL)
#endif

#ifndef ADC_CLOCK_DIV
#define ADC_CLOCK_DIV 96.f
#endif

#ifndef BUZZER_PIN
#define BUZZER_PIN 21
#endif

#endif

static bool _init_display(ssd1306_t *disp);
static void _init_buttons();
static void _init_leds();
static void _init_mic();
static void _init_audio();
bool init_hardware(ssd1306_t *disp);