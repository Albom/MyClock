# MyClock project

A table clock with temperature and humidity sensor. A 1602 LCD is used.

[![Video on Youtube](https://img.youtube.com/vi/fxhWhRGMY40/0.jpg)](https://www.youtube.com/watch?v=fxhWhRGMY40)

It is based on Atmega8 MCU with Arduino bootloader ([MiniCore](https://github.com/MCUdude/MiniCore "MiniCore")) but Digispark with Attiny85 can also be used (not fully compatible yet).

I2C bus is used for connection of LCD and AM2320 sensor. A photoresistor is used for controlling adaptive backlighting. 