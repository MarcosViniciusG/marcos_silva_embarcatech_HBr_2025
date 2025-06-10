# RTOS demo

## Project Objective

The core objective of this project is to develop a concurrent, multitasking embedded system on the BitDogLab board, which is powered by a Raspberry Pi Pico W. By using the FreeRTOS real-time operating system, the project aims to control multiple hardware peripherals simultaneously. The key learning goals include creating and managing tasks, understanding task scheduling and priorities, using essential FreeRTOS API functions like `vTaskCreate()`, `vTaskSuspend()`, and `vTaskResume()`, and controlling GPIO pins for devices such as an RGB LED and a buzzer in a non-blocking manner.

## Used Components

The project utilizes BitDogLab a custom board based on the Raspberry Pi Pico W, incorporating various external components:

* **Microcontroller:** Raspberry Pi Pico W (RP2040)
* **Visual Output:** Common Cathode RGB LED
* **Audio Output:** Passive Buzzer A
* **User Input Buttons:**
    * Button A
    * Button B

## Pinout of the Devices Used

The following table details the connections between the Raspberry Pi Pico W and the components on the custom BitDogLab board:

| Raspberry Pi Pico W GPIO | Component Connected | Notes |
| :--- | :--- | :--- |
| **GPIO 5** | Button A | User input, active low (internal pull-up)  |
| **GPIO 6** | Button B | User input, active low (internal pull-up)  |
| **GPIO 11** | RGB LED (Blue) | Through 220-ohm resistor (common cathode) |
| **GPIO 12** | RGB LED (Red) | Through 220-ohm resistor (common cathode) |
| **GPIO 13** | RGB LED (Green) | Through 220-ohm resistor (common cathode) |
| **GPIO 21** | Buzzer A | Passive buzzer (via transistor). |

## Images and/or videos of the project in operation

*[Link to video demonstration of the project in operation]()*

## Expected and Obtained Results

### Expected Results

* **Concurrent Operation:** The system is expected to run three tasks concurrently: one for the RGB LED, one for the buzzer, and one for monitoring the buttons.
* **RGB LED Task:** The RGB LED should cycle through red, green, and blue, with each color displayed for 500 milliseconds.
* **Buzzer Task:** The buzzer should emit a short beep every 1 second.
* **Button A Control:** When Button A is pressed, the LED task should be suspended, causing the LED to freeze on its current color. When pressed again, the task should resume, and the LED color cycling should continue from where it left off.
* **Button B Control:** When Button B is pressed, the buzzer task should be suspended, silencing the periodic beeps. When pressed again, the task should resume, and the beeping should continue.
* **System Stability:** The system should remain stable, with button presses for one peripheral not affecting the operation of the other.

### Obtained Results

* The project was successfully implemented and operates as expected. All three tasks (LED, Buzzer, Buttons) run concurrently without interfering with one another.
* The `vTaskSuspend()` and `vTaskResume()` functions work correctly, allowing Button A to pause and resume the LED task and Button B to pause and resume the buzzer task.
* The polling method for the buttons, with a 100ms delay, proved effective and did not noticeably impact the performance of the other tasks.

## FAQ

> What happens if all tasks have the same priority?

When all tasks have the same priority, the FreeRTOS scheduler uses an algorithm called **Round-Robin with Time-Slicing**. This means it allocates a small slice of time (a system "tick") to each task, executing them in a cycle. After a task's time expires, the scheduler pauses it and passes control to the next ready task in the queue, ensuring that all have an equal chance to use the CPU and creating an illusion of parallel execution.

***

> Which task consumes the most CPU time?

The task that monitors the **buttons (polling)** is the one that consumes the most CPU time.

This occurs because it happens every 100ms to check the state of the buttons, a much higher frequency than the rest of the tasks.

***

> What are the risks of using polling without priorities?

Using polling in a system without proper priority management presents two main risks:

**Delays and Loss of Responsiveness:** In more complex systems, an intensive polling task could "starve" other tasks, causing the system to become slow or miss critical events.