#pragma once

#include "Arduino.h"
//#include <USBHost_t36.h>
#include <MIDI.h>

#define LP_RED    5
#define LP_BLUE   41
#define LP_GREEN  22
#define LP_DARKGREEN 123
#define LP_PURPLE 49
#define LP_CYAN   33
#define LP_ORANGE 9
#define LP_PINK   57
#define LP_GHOST  1
#define LP_OFF    0
#define LP_DARKBLUE 45
#define LP_YELLOW 13

class LaunchPad {
  private:
    uint8_t _LP_id;
    
    MIDIDevice_BigBuffer *_midiDevice = nullptr;
  public:
    LaunchPad(uint8_t LP_id);
    bool shift;
    uint8_t page;
    uint8_t xZoomLevel;
    uint8_t yScrollStep;
    void begin(MIDIDevice_BigBuffer *midiDevice);
    void setProgrammerMode();
    void setPadColor(uint8_t pad, uint8_t color);
    void setPadColorRGB(uint8_t padId, uint8_t R, uint8_t G, uint8_t B);
    void setPadColorFlashing(uint8_t padId, uint8_t color);
    void setMultiplePadColorState(uint8_t padStateArray[], uint8_t arrayLength);
    void setColumnColor(uint8_t column, uint8_t color);
    void setRowColor(uint8_t row, uint8_t color);
    void setSliderHorizontal(uint8_t row, uint8_t rightmostPad, uint8_t color);
    void setSliderVertical(uint8_t column, uint8_t topmostPad, uint8_t color);
    void resetPage();
    void setActiveTrack(uint8_t pad, uint8_t color);
    void setPageIndicator(uint8_t page, uint8_t color);
    void drawGhostLineLower(uint8_t page, uint8_t color);
};

uint8_t LPnoteToPadColumn(byte note);

uint8_t LPnoteToPadRow(byte note);
