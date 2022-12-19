#include "launchPad.h"
#include "Arduino.h"

LaunchPad::LaunchPad(uint8_t LP_id)
{
  // constructor code here
  _LP_id = LP_id;
  shift = false;
  page = 0;
  xZoomLevel = 4;
  yScrollStep = 4;
}

void LaunchPad::begin(MIDIDevice_BigBuffer *midiDevice)
{
  _midiDevice = midiDevice;
}

void LaunchPad::setProgrammerMode()
{
  uint8_t sysexData[] = {0, 32, 41, 2, 13, 0, 127};
  _midiDevice->sendSysEx(7, sysexData, false);
  _midiDevice->send_now();
}

void LaunchPad::setColumnColor(uint8_t column, uint8_t color)
{
  uint8_t sysexData[] = {0, 32, 41, 2, 13, 3, 0, (uint8_t)(11 + column), color, 0, (uint8_t)(21 + column), color, 0, (uint8_t)(31 + column), color, 0, (uint8_t)(41 + column), color, 0, (uint8_t)(51 + column), color, 0, (uint8_t)(61 + column), color, 0, (uint8_t)(71 + column), color, 0, (uint8_t)(81 + column), color};
  _midiDevice->sendSysEx(sizeof(sysexData), sysexData, false);
}

void LaunchPad::setRowColor(uint8_t row, uint8_t color)
{
  uint8_t sysexData[] = {0, 32, 41, 2, 13, 3, 0, (uint8_t)(11 + row * 10), color, 0, (uint8_t)(11 + row * 10 + 1), color, 0, (uint8_t)(11 + row * 10 + 2), color, 0, (uint8_t)(11 + row * 10 + 3), color, 0, (uint8_t)(11 + row * 10 + 4), color, 0, (uint8_t)(11 + row * 10 + 5), color, 0, (uint8_t)(11 + row * 10 + 6), color, 0, (uint8_t)(11 + row * 10 + 7), color};
  _midiDevice->sendSysEx(sizeof(sysexData), sysexData, false);
}

void LaunchPad::setPadColor(uint8_t padId, uint8_t color)
{
  uint8_t sysexData[] = {0, 32, 41, 2, 13, 3, 0, padId, color};
  _midiDevice->sendSysEx(9, sysexData, false);
}

void LaunchPad::setPadColorRGB(uint8_t padId, uint8_t R, uint8_t G, uint8_t B)
{
  uint8_t sysexData[] = {0, 32, 41, 2, 13, 3, 3, padId, R, G, B};
  _midiDevice->sendSysEx(11, sysexData, false);
}

void LaunchPad::setMultiplePadColorState(uint8_t padStateArray[], uint8_t arrayLength)
{
  uint8_t sysexData[arrayLength + 6];
  uint8_t sysexHeader[6] = {0, 32, 41, 2, 13, 3};
  memcpy(sysexData, sysexHeader, 6);
  memcpy(&sysexData[6], padStateArray, arrayLength);
  _midiDevice->sendSysEx(arrayLength + 6, sysexData, false);
}

void LaunchPad::setPadColorFlashing(uint8_t padId, uint8_t color)
{
  // uint8_t sysexData[] = {0, 32, 41, 2, 13, 3, 2, padId, color};  // pulsing
  uint8_t sysexData[] = {0, 32, 41, 2, 13, 3, 1, padId, color, LP_OFF};  // flashing
  _midiDevice->sendSysEx(10, sysexData, false);
}

void LaunchPad::resetPage()
{
  for (uint8_t column = 0; column < 7; column++)
  {
    setColumnColor(column, LP_OFF);
  }
}

void LaunchPad::setSliderHorizontal(uint8_t row, uint8_t rightmostPad, uint8_t color)
{
  rightmostPad = constrain(rightmostPad, 0, 7);
  for (uint8_t column = 0; column <= rightmostPad; column++)
  {
    uint8_t padId = 11 + 10 * row + column;
    setPadColor(padId, color);
  }
  for (uint8_t column = rightmostPad + 1; column < 8; column++)
  {
    uint8_t padId = 11 + 10 * row + column;
    setPadColor(padId, LP_OFF);
  }
}

void LaunchPad::setPageIndicator(uint8_t page, uint8_t color)
{
  for (uint8_t column = 0; column <= page; column++)
  {
    uint8_t padId = 11 + 10 * 6 + column;
    setPadColor(padId, color);
  }
}

void LaunchPad::setSliderVertical(uint8_t column, uint8_t topmostPad, uint8_t color){}

void LaunchPad::setActiveTrack(uint8_t pad, uint8_t color)
{
  setPadColor(29, LP_OFF);
  setPadColor(39, LP_OFF);
  setPadColor(49, LP_OFF);
  setPadColor(59, LP_OFF);
  setPadColor(69, LP_OFF);
  setPadColor(79, LP_OFF);
  setPadColor(89, LP_OFF);
  
  setPadColor(pad, color);
}

// non-class LP functions
uint8_t LPnoteToPadColumn(byte note)
{
  return note % 10 - 1;
}

uint8_t LPnoteToPadRow(byte note)
{
  return note/10 - 1;
}
