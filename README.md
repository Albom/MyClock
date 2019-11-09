# MyClock project

A table clock with temperature and humidity sensor. A 1602 LCD is used.

Video on Youtube:
[![Video on Youtube](https://img.youtube.com/vi/fxhWhRGMY40/0.jpg)](https://www.youtube.com/watch?v=fxhWhRGMY40)

It is based on Atmega8 MCU with Arduino bootloader ([MiniCore](https://github.com/MCUdude/MiniCore "MiniCore")) but Digispark with Attiny85 can also be used (not fully compatible yet).

I2C bus is used for connection of LCD and AM2320 sensor. A photoresistor is used for controlling adaptive backlighting.

## Setup

MyClock has no buttons and no IR sensor for control. To set date and time you need to load another firmware (MyClockAtmega8Setup.ino or MyClockDigisparkSetup.ino respectively). This firmware uses \_\_DATE\_\_ and \_\_TIME\_\_ constats that contain date and time of sources compilation. If uploading failed you need to restart Arduino IDE for rebuilding the project (incremental building is used). Then upload main firmware (MyClockAtmega8.ino or MyClockDigispark.ino).
