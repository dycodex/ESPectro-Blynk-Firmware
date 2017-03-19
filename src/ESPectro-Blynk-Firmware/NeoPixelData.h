#ifndef __NEOPIXEL_COLOR_H__
#define __NEOPIXEL_COLOR_H__

#include <c_types.h>

class NeoPixelData {
public:
  NeoPixelData();

  void setRed(uint8_t red = 0);
  void setGreen(uint8_t green = 0);
  void setBlue(uint8_t blue = 0);
  void setState(int state = 0);
  uint32_t getColor();
  int getState();

private:
  int currentState = 0;
  uint8_t redValue = 0;
  uint8_t greenValue = 0;
  uint8_t blueValue = 0;
  uint32_t currentColor = 0;

  void updateColor();
};

#endif