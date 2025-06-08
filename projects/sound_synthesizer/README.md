# Sound Synthesizer

## Project objective

The main objective of this project is to develop an Audio Synthesizer using the Raspberry Pi Pico W microcontroller, capable of recording and reproducing digital audio. This involves mastering concepts related to audio signal acquisition, storage, processing, and reproduction. Key aspects include understanding Analog-to-Digital Converters (ADC), Pulse Width Modulation (PWM) for signal generation, and efficient memory manipulation (RAM). The synthesizer interacts with the user through physical buttons, provides visual feedback via an RGB LED, and displays the audio waveform on an OLED screen.

## Used components

The project utilizes BitDogLab a custom board based on the Raspberry Pi Pico W, incorporating various external components:

* **Microcontroller:** Raspberry Pi Pico W (RP2040)
* **Audio Input:** Electret Microphone Module (Analog output)
* **Audio Output:** Passive Buzzer A
* **User Input Buttons:**
    * Button A
    * Button B
* **Visual Feedback:** Common Cathode RGB LED.
    * Red electrode
    * Green electrode
* **Display:** 0.96-inch OLED display (128x64 pixels, I2C communication).

## Pinout of the devices used

The following table details the connections between the Raspberry Pi Pico W and the components on the custom BitDogLab board:

| Raspberry Pi Pico W GPIO | Component Connected | Notes                                         |
| :----------------------- | :------------------ | :-------------------------------------------- |
| **GPIO 5** | Button A            | User input, active low (internal pull-up)     |
| **GPIO 6** | Button B            | User input, active low (internal pull-up)     |
| **GPIO 12** | RGB LED (Red)       | Through 220-ohm resistor (common cathode)     |
| **GPIO 13** | RGB LED (Green)     | Through 220-ohm resistor (common cathode)     |
| **GPIO 14** | OLED SDA            | I2C Data Line (for SSD1306 display)           |
| **GPIO 15** | OLED SCL            | I2C Clock Line (for SSD1306 display)          |
| **GPIO 21** | Buzzer A            | Passive buzzer (via transistor)               |
| **GPIO 28 (ADC2)** | Microphone Input    | Analog input from electret microphone module  |

## Images and/or videos of the project in operation

*[Link to video demonstration of the project in operation](https://www.youtube.com/shorts/gIb1d94QPoQ)*

## Expected and obtained results

### Expected Results

* **Audio Recording:**
    * Upon pressing Button A, the RGB LED turns red, indicating active recording.
    * The system records 5 seconds of audio from the electret microphone at a sample rate of 8 kHz.
    * Recording is handled using the ADC.
    * After 5 seconds, recording stops, and the LED turns off.
* **Audio Playback:**
    * Upon pressing Button B, the RGB LED turns green, indicating active playback.
    * The recorded audio is reproduced through Buzzer A.
    * Playback is handled using PWM
    * The playback duration matches the recorded duration (5 seconds).
    * After playback, the LED turns off.
* **User Interface:**
    * Buttons A and B provide clear control over recording and playback functions.
    * The RGB LED provides immediate visual feedback on the system's state (recording/playing).
* **OLED Display:**
    * The 0.96-inch OLED display shows initial welcome messages and instructions.
    * **During audio playback**, the OLED dynamically displays the waveform of the audio being reproduced. For each 4096 samples, 128 samples are displayed in the screen and the current duration.
* **Performance:**
    * Every task is done on the CPU, so the audio recording and playing is a bit chopped and the waveform displayed is slow.

### Obtained Results
* The project is working as expected, but DMA and possibly Multicore should be implemented in the future for a better performance.