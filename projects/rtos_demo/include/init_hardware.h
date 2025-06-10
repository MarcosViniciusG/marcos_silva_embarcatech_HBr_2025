#ifndef _INIT_HARDWARE_H
#define _INIT_HARDWARE_H

#include "pico/stdlib.h"

// --- Buttons pins ---
#ifndef BUTTON_A_PIN
#define BUTTON_A_PIN 5
#endif

#ifndef BUTTON_B_PIN
#define BUTTON_B_PIN 6
#endif

// --- LEDs pins ---
#ifndef RED_LED_PIN
#define RED_LED_PIN 13
#endif

#ifndef BLUE_LED_PIN
#define BLUE_LED_PIN 12
#endif

#ifndef GREEN_LED_PIN
#define GREEN_LED_PIN 11
#endif

// --- Buzzer A pin ---
#ifndef BUZZER_PIN
#define BUZZER_PIN 21
#endif

static void _init_buttons();
static void _init_leds();
static void _init_audio();
void init_hardware();

#endif