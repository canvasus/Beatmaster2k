#include "ui.h"

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);

Encoder encoders[2] = {Encoder(ROT1_A, ROT1_B), Encoder(ROT2_A, ROT2_B)};
Bounce bouncers[2] = {Bounce(ROT1_SW, 5), Bounce(ROT2_SW, 5)}; 
//
//uint8_t displayMode = DISPLAYMODE_SEQUENCER;
//uint8_t currentPage = PAGE_HOME;
//const String pageNames[] = {"HOME", "OSC", "AMP ENV", "FILT ENV", "MOD", "WAVE", "CHORUS", "REVERB", "MIX"};
//// note: make sure this array matches the PAGE_YYY defines
//
//
////                                        name        .value  .minValue .maxValue .multiplier .displayPrecision  .getFunction .setFunction                                          
//const parameters displayParameters[] = { 
//                                         {"OSC V0",   0,      0,        1,        1.0,        0,    nullptr, nullptr},
//                                         {"OSC V1",   0,      0,        1,        1.0,        0,    nullptr, nullptr},
//                                         {"OSC V3",   0,      0,        1,        1.0,        0,    nullptr, nullptr},
//                                         {"OSC V4",   0,      0,        1,        1.0,        0,    nullptr, nullptr},
//                                        };
//


void setupUI()
{
  Serial.println("UI SETUP");
  tft.begin();
  tft.setRotation(3);
  tft.setFont(Arial_18);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(20, 20);
  tft.print("HELLO WORLD!");
  pinMode(ROT1_SW, INPUT_PULLUP);
  pinMode(ROT2_SW, INPUT_PULLUP);
}

void updateUI()
{
  updateDisplayUI();
  updateLaunchpadUI();
}

void updateDisplayUI()
{
  static uint8_t counter = 0;
  tft.fillRect(50, 50, 60, 20, ILI9341_BLUE);
  tft.setCursor(50, 50);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(counter);
  counter++;

  getEncoderDirection(0);
  getEncoderDirection(1);
  updateButton(0);
  updateButton(1);
  
////  static uint8_t oldPage = PAGE_HOME;
////  static bool firstCall = true;
////  switch (currentPage)
////  {
////    case PAGE_HOME:
////      updatePageHome(firstCall);
////      break;
////    case PAGE_MIX:
////      updatePageMix(firstCall);
////      break;
////  }
////  firstCall = !(oldPage == currentPage);
////  oldPage =  currentPage;
}

void updateLaunchpadUI()
{
  
}

int16_t getEncoderDirection(uint8_t encoderNr)
{
  int16_t value = encoders[encoderNr].read();
  int16_t returnValue = 0;
  if (value > 3)
  {
    returnValue = 1;
    encoders[encoderNr].write(0);
    Serial.printf("Encoder %d: %d \n", encoderNr, value);
  }
  if (value < -3)
  {
    returnValue = -1;
    encoders[encoderNr].write(0);
    Serial.printf("Encoder %d: %d \n", encoderNr, value);
  }
  return returnValue;
}

uint8_t updateButton(uint8_t buttonNr)
{
  bouncers[buttonNr].update();
  if (bouncers[buttonNr].fallingEdge())
  {
    Serial.print("Button ");
    Serial.println(buttonNr);
    return 1;
  }
  else return 0;
}
