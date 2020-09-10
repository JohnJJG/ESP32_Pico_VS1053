Adafruit VS1053 Codec Library compatible with ESP32 Pico

This is the Adafruit library with 2 key modifications:
  - Interupt handling of DREQ modified for ESP32 (from https://github.com/danclarke/Adafruit_VS1053_Library)
  - Hardware SPI for user defined pins (ESP32 allows hardware SPI to use almost any pins)
