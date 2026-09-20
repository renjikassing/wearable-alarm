# Wearable Alarm Clock

A wrist-worn alarm that wakes a user with simultaneous haptic and auditory feedback using an ESP32 and a coin motor.

## Hardware
* DOIT ESP32 Devkit V1
* Sunfounder Breadvolt Power Supply 5V/3.3V, 500mAh rechargeable battery
* Passive buzzer
* Coin motor, 1N4007, PN2222, 1kΩ resistor
* Perfboard

## Photos

## Workflow

## Power
It has an approximate battery life of 50 hours.
A user activates the device with the switch located on the power supply, and it begins advertising in Bluetooth Low Energy (BLE). The user connects to it with a BLE app such as LightBlue, and the device will beep to verify successful connection. 
The user then enters the current time as a string in the format "TIME: YYYY-MM-DD hh:mm:ss", and the device will beep to verify successful reception. 
The user then enters the desired alarm time as a string in the format "ALARM: YYYY-MM-DD hh:mm:ss", and the device will beep twice to verify successful reception, calculate the difference between the alarm time and current time, then enter deep sleep mode to conserve power.
The ESP32's RTC stores this wake-up time, reactivates when it is reached, and the wake-up sequence is triggered, where the device vibrates the coin motor and beeper synchronously in a series of long pulses and quick chirps. The device is disabled by flicking the power supply's switch off.
The device's workflow is then reset, and the user may set another alarm time by turning the device back on.

## Future Improvements
