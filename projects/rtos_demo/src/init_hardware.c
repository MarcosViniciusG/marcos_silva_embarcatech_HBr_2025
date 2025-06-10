#include "init_hardware.h"

/**
 * @brief Initializes the GPIO pins for the user buttons.
 * * This function configures the GPIO pins connected to BUTTON_A_PIN and
 * BUTTON_B_PIN as inputs. It also enables the internal pull-up resistors
 * to ensure a stable HIGH signal when the buttons are not being pressed.
 */
static void _init_buttons() 
{
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
}

/**
 * @brief Initializes the GPIO pins for the RGB LEDs.
 * * This function sets up the GPIO pins for the RED, GREEN, and BLUE LEDs
 * as outputs, allowing the program to control their state (on/off).
 */
static void _init_leds() 
{
    gpio_init(RED_LED_PIN);
    gpio_init(GREEN_LED_PIN);
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);
}

/**
 * @brief Initializes the GPIO pin for the audio buzzer.
 * * This function configures the GPIO pin connected to the BUZZER_PIN
 * as an output, which enables the system to generate sounds.
 */
static void _init_audio() 
{
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
}

/**
 * @brief Main hardware initialization function.
 * * This public function serves as a single entry point to initialize all 
 * the hardware components of the device. It calls the respective private 
 * initialization routines for the buttons, LEDs, and audio output.
 */
void init_hardware() 
{
    _init_buttons();
    _init_leds();
    _init_audio();
}