#include "ui.h"
//
//Adafruit_SH1106G oled = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);
//Encoder encoders[2] = {Encoder(ROT1_A, ROT1_B), Encoder(ROT2_A, ROT2_B)};
//Bounce bouncers[5] = {Bounce(ROT1_SW, 5), Bounce(ROT2_SW, 5), Bounce(SW_A, 9), Bounce(SW_B, 9), Bounce(SW_C, 9)}; 
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
//bool radiusChangeMode = false;
//
//void setupUI()
//{
//  Serial.println("UI SETUP");
//  pinMode(ROT1_SW, INPUT_PULLUP);
//  pinMode(ROT2_SW, INPUT_PULLUP);
//  pinMode(SW_A, INPUT_PULLUP);
//  pinMode(SW_B, INPUT_PULLUP);
//  pinMode(SW_C, INPUT_PULLUP);
//  oled.begin(SCREEN_ADDRESS, true);
//  oled.clearDisplay();
//  oled.display();
//}
//
//void updateUI()
//{
//  updateDisplayUI();
//  updateLaunchpadUI();
//}
//
//
//void updateDisplayUI()
//{
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
//}
//
//void updateLaunchpadUI()
//{
//  
//}
//
//// Common update functions
////**********************************
//
//void updateNavigationPage(const uint8_t * pageArray, uint8_t nrButtons, bool firstCall)
//{
//  static int16_t selectedButton = 0;
//  if (firstCall)
//  {
//    oled.clearDisplay();
//    selectedButton = 0;
//    encoders[0].write(selectedButton);
//    for (uint8_t i = 0; i < nrButtons; i++) { drawMenuButton(i, pageNames[pageArray[i]], i == 0); }
//    oled.display();
//  }
//  else
//  {
//    int16_t newSelectedButton = selectedButton + getEncoderDirection(0);
//    Serial.println(newSelectedButton);
//    if (newSelectedButton < 0) newSelectedButton = nrButtons - 1;
//    if (newSelectedButton > nrButtons - 1) newSelectedButton = 0;
//    if (newSelectedButton != selectedButton)
//    {
//      drawMenuButton(selectedButton, pageNames[pageArray[selectedButton]], false);
//      selectedButton = newSelectedButton;
//      drawMenuButton(selectedButton,  pageNames[pageArray[selectedButton]], true);
//      oled.display();
//    }
//    if (updateButton(2)) currentPage = pageArray[selectedButton];
//  }
//}
//
//void drawMenuButton(uint8_t index, String text, bool selected)
//{
//  uint8_t row = index % 4;
//  uint8_t column = index / 4 ;
//  uint8_t x1 = column * 64 + 1;
//  uint8_t y1 = row * 16 + 1;
//  const uint8_t w = 62;
//  const uint8_t h = 14;
//  if (selected)
//  {
//    oled.fillRect(x1, y1, w, h, SH110X_WHITE);
//    oled.setTextColor(SH110X_BLACK);
//  }
//  else
//  {
//    oled.fillRect(x1, y1, w, h, SH110X_BLACK);
//    oled.drawRect(x1, y1, w, h, SH110X_WHITE);
//    oled.setTextColor(SH110X_WHITE);
//  }
//  oled.setCursor(column * 64 + 8, row * 16 + 5);
//  oled.print(text);
//}
//
//void updateParameterPage(const uint8_t * parameterArray, const uint8_t nrParameters, bool firstCall)
//{
//  static int16_t selectedButton = 0;
//  float variables[nrParameters];
//  for (uint8_t i = 0; i < nrParameters; i++) variables[i] = getValue(parameterArray[i]);
// 
//  if (firstCall)
//  {
//    oled.clearDisplay();
//    selectedButton = 0;
//    encoders[0].write(selectedButton);
//    for (uint8_t i = 0; i < nrParameters; i++) drawParameterRow(i, displayParameters[parameterArray[i]].name, variables[i], displayParameters[parameterArray[i]].displayPrecision, i == 0);
//    oled.display();
//  }
//  else
//  {
//    int16_t newSelectedButton = selectedButton + getEncoderDirection(0);
//    if (newSelectedButton < 0) newSelectedButton = nrParameters - 1;
//    if (newSelectedButton > nrParameters - 1) newSelectedButton = 0;
//    if (newSelectedButton != selectedButton)
//    {
//      drawParameterRow(selectedButton, displayParameters[parameterArray[selectedButton]].name, variables[selectedButton], displayParameters[parameterArray[selectedButton]].displayPrecision, false);
//      selectedButton = newSelectedButton;
//      drawParameterRow(selectedButton, displayParameters[parameterArray[selectedButton]].name, variables[selectedButton], displayParameters[parameterArray[selectedButton]].displayPrecision, true);
//      encoders[1].write(0);
//      oled.display();
//    }
//
//    int16_t deltaBaseValue = encoders[1].read() / 4;
//    float newValue = 0.0;
//    if (abs(deltaBaseValue) > 0)
//    {
//      encoders[1].write(0);
//      newValue = constrain(variables[selectedButton] + deltaBaseValue * displayParameters[parameterArray[selectedButton]].multiplier, displayParameters[parameterArray[selectedButton]].minValue, displayParameters[parameterArray[selectedButton]].maxValue);
//      updateValue(parameterArray[selectedButton], newValue);
//      updateParameterRow(selectedButton, newValue , displayParameters[parameterArray[selectedButton]].displayPrecision);
//      oled.display();
//    }
//  }
//  if (updateButton(4)) currentPage = PAGE_HOME;
//}
//
//
//float getValue(uint8_t parameter)
//{
//  if (displayParameters[parameter].getFunction != nullptr) return displayParameters[parameter].getFunction();
//  else return 0.0;
//}
//
//void updateValue(uint8_t parameter, float value)
//{
//  if (displayParameters[parameter].setFunction != nullptr) displayParameters[parameter].setFunction(value);
//}
//
//void drawParameterRow(uint8_t index, String name, float value, uint8_t precision, bool selected)
//{
//  uint8_t row = index % 4;
//  uint8_t x1 = 1;
//  uint8_t y1 = row * 16 + 1;
//  if (selected)
//  {
//    oled.fillRect(x1, y1, BUTTON_WIDTH, BUTTON_HEIGHT, SH110X_WHITE);
//    oled.setTextColor(SH110X_BLACK);
//  }
//  else
//  {
//    oled.fillRect(x1, y1, BUTTON_WIDTH, BUTTON_HEIGHT, SH110X_BLACK);
//    oled.drawRect(x1, y1, BUTTON_WIDTH, BUTTON_HEIGHT, SH110X_WHITE);
//    oled.setTextColor(SH110X_WHITE);
//  }
//  oled.setCursor(8, row * 16 + 5);
//  oled.print(name);
//  oled.drawRect(x1 + 64, y1, BUTTON_WIDTH, BUTTON_HEIGHT, SH110X_WHITE);
//  oled.setTextColor(SH110X_WHITE);
//  oled.setCursor(8 + 64, row * 16 + 5);
//  oled.print(value, precision);
//}
//
//void updateParameterRow(uint8_t index, float value, uint8_t precision)
//{
//   uint8_t row = index % 4;
//   uint8_t x1 = 1;
//   uint8_t y1 = row * 16 + 1;
//   oled.fillRect(x1 + 64, y1, BUTTON_WIDTH, BUTTON_HEIGHT, SH110X_BLACK);
//   oled.drawRect(x1 + 64, y1, BUTTON_WIDTH, BUTTON_HEIGHT, SH110X_WHITE);
//   oled.setCursor(8 + 64, row * 16 + 5);
//   oled.print(value, precision);
//}
//
//void drawParameterRowEnum(uint8_t index, String name, String value, bool selected)
//{
//  uint8_t row = index % 4;
//  uint8_t x1 = 1;
//  uint8_t y1 = row * 16 + 1;
//  if (selected)
//  {
//    oled.fillRect(x1, y1, BUTTON_WIDTH, BUTTON_HEIGHT, SH110X_WHITE);
//    oled.setTextColor(SH110X_BLACK);
//  }
//  else
//  {
//    oled.fillRect(x1, y1, BUTTON_WIDTH, BUTTON_HEIGHT, SH110X_BLACK);
//    oled.drawRect(x1, y1, BUTTON_WIDTH, BUTTON_HEIGHT, SH110X_WHITE);
//    oled.setTextColor(SH110X_WHITE);
//  }
//  oled.setCursor(8, row * 16 + 5);
//  oled.print(name);
//  oled.drawRect(x1 + 64, y1, BUTTON_WIDTH, BUTTON_HEIGHT, SH110X_WHITE);
//  oled.setTextColor(SH110X_WHITE);
//  oled.setCursor(8 + 64, row * 16 + 5);
//  oled.print(value);
//}
//
//void updateParameterRowEnum(uint8_t index, String value);
//
//// PAGE FUNCTIONS START HERE
//// *******************************************
//
//void updatePageHome(bool firstCall)
//{
//  const uint8_t pageArray[] = {PAGE_OSC, PAGE_AMPENV, PAGE_FILTERENV, PAGE_MOD, PAGE_CHORUS, PAGE_REVERB, PAGE_MIX};
//  uint8_t nrButtons = sizeof(pageArray);
//  updateNavigationPage(pageArray, nrButtons, firstCall);
//}
//
//void updatePageMix(bool firstCall)
//{
//  const uint8_t parameterArray[] = {MIX_DRY, MIX_CHORUS, MIX_REVERB};
//  const uint8_t nrParameters = sizeof(parameterArray);
//  updateParameterPage(parameterArray, nrParameters, firstCall);
//}
//
//
//
//int16_t getEncoderDirection(uint8_t encoderNr)
//{
//  int16_t value = encoders[encoderNr].read();
//  int16_t returnValue = 0;
//  if (value > 3)
//  {
//    returnValue = 1;
//    encoders[encoderNr].write(0);
//  }
//  if (value < -3)
//  {
//    returnValue = -1;
//    encoders[encoderNr].write(0);
//  }
//  return returnValue;
//}
//
//uint8_t updateButton(uint8_t buttonNr)
//{
//  bouncers[buttonNr].update();
//  if (bouncers[buttonNr].fallingEdge())
//  {
//    Serial.print("Button ");
//    Serial.println(buttonNr);
//    return 1;
//  }
//  else return 0;
//}
