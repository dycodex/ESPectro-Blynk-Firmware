ESPectro Blynk Firmware
=======================

Using ESPectro with Blynk with almost no coding required.

### IDE Compatibility

* Arduino IDE
* PlatformIO

### Dependencies

You have to install the following libraries:

* [Blynk](https://github.com/blynkkk/blynk-library/releases/latest)
* [ESPectro](https://github.com/andriyadi/EspX)
* [NeoPixelBus](https://github.com/Makuna/NeoPixelBus)
* [WiFiManager](https://github.com/tzapu/WiFiManager)
* [Adafruit BMP085 Library](https://github.com/adafruit/Adafruit-BMP085-Library)

However, if you're familiar with PlatformIO, you only need to download Adafruit's BMP085 Library manually to `lib` directory!

### Blynk's Virtual Pins

We use Blynk Virtual Pins for:

* Controlling NeoPixel + adjust LED color.
* Reading Adafruit BMP sensor data (temperature, humidity, pressure).
* Set analog reading threshold. If the reading is below the threshold, the firmware will attempt to send HIGH value to GPIO 10 (used to toggle ConnectIO).

Here's the pin mapping:

Virtual Pin No. | Used as | Read / Write
----------------|---------|-------------
0 | NeoPixel LED switch | Write
1 | NeoPixel red color (0 - 255) | Write
2 | NeoPixel green color (0 - 255) | Write
3 | NeoPixel blue color (0 - 255) | Write
4 | Temperature | Read
5 | Pressure | Read
6 | Altitude | Read
7 | Sea-level Pressure | Read
8 | Analog reading (A0) threshold | Write
