#include "NeoPixelData.h"

NeoPixelData::NeoPixelData() {}

void NeoPixelData::setRed(uint8_t red) {
  redValue = red;
  updateColor();
}

void NeoPixelData::setGreen(uint8_t green) {
  greenValue = green;
  updateColor();
}

void NeoPixelData::setBlue(uint8_t blue) {
  blueValue = blue;
  updateColor();
}

uint32_t NeoPixelData::getColor() {
  return currentColor;
}

void NeoPixelData::updateColor() {
  currentColor = blueValue | greenValue << 8 | redValue << 16;
}

int NeoPixelData::getState() {
  return currentState;
}

void NeoPixelData::setState(int state) {
  currentState = state;
}
