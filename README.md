# Wearable Alarm Clock

A wrist-worn alarm that wakes a user with simultaneous haptic and auditory feedback, powered by an ESP32 and a coin motor.

## Hardware
* DOIT ESP32 Devkit V1
* Sunfounder Breadvolt Power Supply 5V/3.3V, 500mAh rechargeable battery
* Passive buzzer
* Coin motor, 1N4007, PN2222, 1kΩ resistor
* Perfboard

## Schematic
<p align="center">
  <img src="images/schematic.png" width="500">
</p>

## Photos
<p align="center">
  <img src="images/covered.jpg" width="45%">
  <img src="images/no_cover.jpg" width="45%">
</p>
<p align="center">
  <img src="images/perfboard.jpg" width="400">
</p>

## Workflow
1. The user activates the device with the power supply switch.
2. The alarm begins advertising over Bluetooth Low Energy (BLE).
3. The user connects to the device with a BLE app such as LightBlue.
4. The user enters the current time as a string in the format "TIME: YYYY-MM-DD hh:mm:ss".
5. They then enter the desired alarm time as a string in the format "TIME: YYYY-MM-DD hh:mm:ss".
6. The ESP32 calculates the difference between the current time and alarm time, configures its RTC wake-up timer to this value, then enters deep sleep.
7. Upon waking, the alarm sequence activates, simultaneously driving the coin motor and buzzer in a series of long pulses and quick chirps.
8. The alarm is disabled by turning off the power supply.
9. The device's workflow is now reset, and a new alarm may be configured upon powering on.

## Power
The device uses a 500mAh battery and has an approximate battery life of 50 hours. Deep sleep mode is used to conserve power.

## Software
The ESP32 firmware is written in C++ within the Arduino framework. The source code is included in this repository as "wearable_alarm_flash.ino".
The software handles:
* BLE Communication with the cell phone
* Parsing of the current and alarm time
* Alarm time calculation
* Power management through deep sleep mode
* Synchronized motor and buzzer control

## Future Improvements
1. The power supply module and ESP32 Devkit are intended for breadboarding and could be reduced in size to improve the user experience. Additionally, both modules contain circuitry unrelated to the alarm's operation, so more intentional hardware would also reduce power consumption.
2. Both modules have permanently enabled LEDs that consume a combined 5.5 mA, measured experimentally. Desoldering these LEDs is a minimally invasive way to reduce power consumption.
3. Creating an app for the alarm would remove the need for the user to input the current time and require less strict formatting for the alarm time.
