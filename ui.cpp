#include "ui.h"

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
Encoder encoders[2] = {Encoder(ROT1_A, ROT1_B), Encoder(ROT2_A, ROT2_B)};
Bounce bouncers[2] = {Bounce(ROT1_SW, 5), Bounce(ROT2_SW, 5)}; 
MCP23017 mcp = MCP23017(MCP23017_ADDR, Wire1);

uint8_t currentTrack = 0;
uint8_t currentPattern = 0;
int16_t currentEvent = -1;
uint8_t currentPage = PAGE_SONG;
uint8_t LPdisplayMode = LPMODE_PATTERN;

//                      .pageType .name       .pageBack   .nrChildren  .children  .widget
const page pages[] = {
                      {PAGE_PAR,  "SONG",     PAGE_SONG,  1,         {SONG_BPM}, W_LIST},
                      {PAGE_PAR,  "PATTERN",  PAGE_PATTERN,  3,      {PATTERN_LENGTH, PATTERN_SPEED, PATTERN_EVENTLENGTHDEF}, W_LIST},
                      {PAGE_PAR,  "EVENT",    PAGE_EVENT, 1,         {EVENT_LENGTH}, W_LIST},
                      {PAGE_PAR,  "TRACK",    PAGE_TRACK,  3,        {TRACK_CHANNEL, TRACK_OUTPUT, TRACK_TRANSPOSESTATUS}, W_LIST},
                      {PAGE_PAR,  "FILE",     PAGE_FILE,  0,         {}, W_NONE},
                      {PAGE_LISTDEVICES,  "USBDEV",      PAGE_LISTDEVICES,  1,         {}, W_LIST}
               
};

//                                        name        .value  .minValue .maxValue .multiplier .displayPrecision  .getFunction .setFunction  .enumFunction                                          
const parameters displayParameters[] = { 
                                         {"LENGTH",   0,      1,        128,       1,         0,    &getTrackLengthColumns, &setTrackLengthColumns, nullptr},
                                         {"SPEED",    0,      0.5,      2,        0.5,        1,    nullptr, nullptr, nullptr},
                                         {"DEF.LEN",  0,      0,        64,        1,         0,    nullptr, nullptr, nullptr},
                                         {"OUTPUT",   0,      0,        6,         1,         0,    &getTrackOutput, &setTrackOutput, &getOutputEnum},
                                         {"BPM",      0,      20,       300,       1,         0,    &getBpm, &setBpm, nullptr},
                                         {"CHANNEL",  0,      0,        16,        1,         0,    &getTrackChannel, &setTrackChannel, nullptr},
                                         {"LENGTH",  0,      0,        128,        1,         0,    &getEventLength, &setEventLength, nullptr},
                                         {"REC.TRP",  0,      0,        2,        1,         0,     &getTransposeStatus, &setTransposeStatus, &getNoYesSelectedEnum}
                                         
                                        };


void setupUI()
{
  Serial.println("UI SETUP");
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  showStartupScreen();
  pinMode(ROT1_SW, INPUT_PULLUP);
  pinMode(ROT2_SW, INPUT_PULLUP);
  initMcp();
  
  updateHeader(currentTrack, currentPattern, 120, true);
}

void showStartupScreen()
{
  tft.setFont(Arial_24);
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(20, 90);
  tft.print("BEATMAST3R 2K");
  for (uint8_t i = 0; i < 16; i++)
  {
    tft.drawFastHLine(10, 80 - i * 4, 300, tft.color565(0,200 - i * 10, 0));
    tft.drawFastHLine(10, 120 + i * 4, 300, tft.color565(0,200 - i * 10, 0));
    delay(200);
  }
  tft.fillScreen(ILI9341_BLACK);
}

void updateUI()
{
  static elapsedMillis mcpTimer = 0;
  updateDisplayUI();
  updateLaunchpadUI();
  if (mcpTimer > 100)
  {
    mcpTimer = 0;
    updateMcp();
  }
  
}

// DISPLAY UI FUNCTIONS
// ********************************************

void updateDisplayUI()
{
  static uint8_t oldPage = PAGE_SONG;
  static bool firstCall = true;
  static uint8_t forceVariableUpdate = false;
  if (pages[currentPage].pageType == PAGE_NAV) updateNavigationPage(pages[currentPage].children, pages[currentPage].nrChildren, firstCall);
  else if (pages[currentPage].pageType == PAGE_PAR) updateParameterPage(pages[currentPage].children, pages[currentPage].nrChildren, firstCall, forceVariableUpdate);
  else handleSpecialPages(firstCall);
  firstCall = !(oldPage == currentPage);
  oldPage =  currentPage;
  forceVariableUpdate = updateHeader(currentTrack, currentPattern, bpm, false);
}

bool updateHeader(uint8_t trackNr, uint8_t patternNr, uint8_t bpm, bool firstCall)
{
  static uint8_t oldTrackNr = 255;
  static uint8_t oldPatternNr = 255;
  static uint8_t oldBpm = 255;
  static uint8_t oldPage = 255;
  static uint8_t oldLPpage = 255;
  static uint8_t oldTrackColumns = 255;
  if (firstCall)
  {
    tft.fillRect(HEADER_X, HEADER_Y, HEADER_WIDTH, HEADER_HEIGHT, ILI9341_LIGHTGREY);
    tft.setTextColor(ILI9341_BLACK);
    tft.setFont(Arial_12);
    tft.setCursor(HEADER_TEXT_X, 5 + 0* HEADER_OFFSET_Y);
    tft.print("TRK");
    tft.setCursor(HEADER_TEXT_X, 5 + 2* HEADER_OFFSET_Y);
    tft.print("PTN");
    tft.setCursor(HEADER_TEXT_X, 5 + 4* HEADER_OFFSET_Y);
    tft.print("BPM");
    tft.setCursor(HEADER_TEXT_X, 5 + 7* HEADER_OFFSET_Y);
    tft.print("PAG");
  }
  if (oldPage != currentPage)
  {
    oldPage = currentPage;
    tft.fillRect(0, 0, PAGEINDICATOR_WIDTH, PAGEINDICATOR_HEIGHT, ILI9341_BLUE);
    tft.setCursor(3, 6);
    tft.setFont(Arial_14);
    tft.setTextColor(ILI9341_WHITE);
    tft.print(pages[currentPage].name);
  }
  if (oldTrackNr != trackNr)
  {
    tft.fillRect(HEADER_TEXT_X, 5 + HEADER_OFFSET_Y, HEADER_WIDTH, HEADER_OFFSET_Y - 1, ILI9341_LIGHTGREY);
    tft.fillRect(HEADER_TEXT_X + HEADER_WIDTH - 16, HEADER_OFFSET_Y, 16, HEADER_OFFSET_Y - 16, lpColor2tftColor(tracks[currentTrack]->color));
    tft.setCursor(HEADER_TEXT_X + 16, 5 + HEADER_OFFSET_Y);
    tft.setFont(Arial_14);
    tft.setTextColor(ILI9341_BLACK);
    tft.print(trackNr);
    oldTrackNr = trackNr;
    return true;
  }
  if (oldPatternNr != patternNr)
  {
    tft.fillRect(HEADER_TEXT_X, 5 + 3* HEADER_OFFSET_Y, HEADER_WIDTH, HEADER_OFFSET_Y - 1, ILI9341_LIGHTGREY);
    tft.setCursor(HEADER_TEXT_X, 5 + 3* HEADER_OFFSET_Y);
    tft.setFont(Arial_14);
    tft.setTextColor(ILI9341_BLACK);
    tft.printf("%02d", patternNr);
    oldPatternNr = patternNr;
    return true;
  }
  if (oldBpm != bpm)
  {
    tft.fillRect(HEADER_TEXT_X, 5 + 5* HEADER_OFFSET_Y, HEADER_WIDTH, HEADER_OFFSET_Y - 1, ILI9341_LIGHTGREY);
    tft.setCursor(HEADER_TEXT_X, 5 + 5* HEADER_OFFSET_Y);
    tft.setFont(Arial_14);
    tft.setTextColor(ILI9341_BLACK);
    tft.printf("%03d", bpm);
    oldBpm = bpm;
  }
  if ( (oldLPpage != LP1.page) || (oldTrackColumns !=  tracks[currentTrack]->getPatternLengthColumns(24 / LP1.xZoomLevel)))
  {
    uint8_t ticksPerColumn = 24 / LP1.xZoomLevel;
    uint8_t trackColumns = tracks[currentTrack]->getPatternLengthColumns(ticksPerColumn);
    uint8_t maxPages = trackColumns/8 + !!(trackColumns%8);
    tft.fillRect(HEADER_TEXT_X, 5 + 8* HEADER_OFFSET_Y, HEADER_WIDTH, HEADER_OFFSET_Y - 1, ILI9341_LIGHTGREY);
    tft.setCursor(HEADER_TEXT_X, 5 + 8* HEADER_OFFSET_Y);
    tft.setFont(Arial_12);
    tft.setTextColor(ILI9341_BLACK);
    tft.printf("%02d/%02d", LP1.page + 1, maxPages);
    oldLPpage = LP1.page;
    oldTrackColumns = trackColumns;
  }
  
  return false;
}

void updateNavigationPage(const uint8_t * pageArray, uint8_t nrButtons, bool firstCall)
{
  static int16_t selectedButton = 0;
  
  if (firstCall)
  {
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_XRES, SCREEN_XRES - HEADER_HEIGHT , ILI9341_BLACK);
    selectedButton = 0;
    encoders[0].write(selectedButton);
    for (uint8_t i = 0; i < nrButtons; i++) { drawMenuButton(i, pages[pageArray[i]].name, i == 0); }
  }
  else
  {
    int16_t newSelectedButton = selectedButton + getEncoderDirection(0);
    if (newSelectedButton < 0) newSelectedButton = nrButtons - 1;
    if (newSelectedButton > nrButtons - 1) newSelectedButton = 0;
    if (newSelectedButton != selectedButton)
    {
      drawMenuButton(selectedButton, pages[pageArray[selectedButton]].name, false);
      selectedButton = newSelectedButton;
      drawMenuButton(selectedButton,  pages[pageArray[selectedButton]].name, true);
    }
    if (updateButton(0)) currentPage = pageArray[selectedButton];
    if (updateButton(1)) currentPage = pages[currentPage].pageBack;
  }
}

void drawMenuButton(uint8_t index, String text, bool selected)
{
  uint8_t row = index % 5;
  uint8_t column = index / 5 ;
  uint8_t x1 = BUTTON_PADDING + column * (VAR_NAME_WIDTH + BUTTON_PADDING);
  uint8_t y1 = PAGEINDICATOR_HEIGHT + BUTTON_PADDING + row * (BUTTON_HEIGHT + BUTTON_PADDING);
  const uint8_t w = VAR_NAME_WIDTH;
  const uint8_t h = BUTTON_HEIGHT;
  if (selected)
  {
    tft.drawRect(x1, y1, w, h, ILI9341_DARKGREY);
    tft.fillRect(x1 + 1, y1 + 1, w - 2, h - 2, ILI9341_WHITE);
    tft.setTextColor(ILI9341_BLUE);
  }
  else
  {
    tft.drawRect(x1, y1, w, h, ILI9341_DARKGREY);
    tft.fillRect(x1 + 1, y1 + 1, w - 2, h - 2, ILI9341_BLUE);
    tft.setTextColor(ILI9341_WHITE);
  }
  tft.setFont(Arial_12);
  tft.setCursor(x1 + 8, y1 + 5);
  tft.print(text);
}


void updateParameterPage(const uint8_t * parameterArray, const uint8_t nrParameters, bool firstCall, bool forceVariableUpdate)
{
  static int16_t selectedButton = 0;
  float variables[nrParameters];

  for (uint8_t i = 0; i < nrParameters; i++)
  {
    variables[i] = getValue(parameterArray[i]);
    if (forceVariableUpdate && !firstCall) updateParameterRow(i, parameterArray[i]);
  }
  
  if (firstCall)
  {
    tft.fillRect(0, PAGEINDICATOR_HEIGHT, HEADER_X - 1, SCREEN_XRES - PAGEINDICATOR_HEIGHT , ILI9341_BLACK);
    selectedButton = 0;
    encoders[0].write(selectedButton);
    if (pages[currentPage].widget == W_LIST) for (uint8_t i = 0; i < nrParameters; i++) drawParameterRow(i, parameterArray[i], i == 0);
    else if (pages[currentPage].widget == W_POT) for (uint8_t i = 0; i < nrParameters; i++) drawPotWidget(i, parameterArray[i], i == 0, true);
  }
  else
  {
    int16_t newSelectedButton = selectedButton + getEncoderDirection(0);
    if (newSelectedButton < 0) newSelectedButton = nrParameters - 1;
    if (newSelectedButton > nrParameters - 1) newSelectedButton = 0;
    if (newSelectedButton != selectedButton)
    {
      if (pages[currentPage].widget == W_LIST) drawParameterRow(selectedButton, parameterArray[selectedButton], false);
      else if (pages[currentPage].widget == W_POT) drawPotWidget(selectedButton, parameterArray[selectedButton], false, true);
      selectedButton = newSelectedButton;
      if (pages[currentPage].widget == W_LIST) drawParameterRow(selectedButton, parameterArray[selectedButton], true);
      else if (pages[currentPage].widget == W_POT) drawPotWidget(selectedButton, parameterArray[selectedButton], true, true);
      encoders[1].write(0);
    }

    int16_t deltaBaseValue = encoders[1].read() / 4;
    float newValue = 0.0;
    if (abs(deltaBaseValue) > 0)
    {
      encoders[1].write(0);
      newValue = constrain(variables[selectedButton] + deltaBaseValue * displayParameters[parameterArray[selectedButton]].multiplier, displayParameters[parameterArray[selectedButton]].minValue, displayParameters[parameterArray[selectedButton]].maxValue);
      updateValue(parameterArray[selectedButton], newValue);
      if (pages[currentPage].widget == W_LIST) updateParameterRow(selectedButton, parameterArray[selectedButton]);
      else if (pages[currentPage].widget == W_POT) drawPotWidget(selectedButton, parameterArray[selectedButton], true, false);
    }
  }
  
  if (updateButton(1)) currentPage = pages[currentPage].pageBack;
}

float getValue(uint8_t parameter)
{
  if (displayParameters[parameter].getFunction != nullptr) return displayParameters[parameter].getFunction();
  else return 0.0;
}

void updateValue(uint8_t parameter, float value)
{
  if (displayParameters[parameter].setFunction != nullptr) displayParameters[parameter].setFunction(value);
}

void drawParameterRow(uint8_t index, uint8_t parameter, bool selected)
{
  drawMenuButton(index, displayParameters[parameter].name, selected);
  updateParameterRow(index, parameter);
}

void updateParameterRow(uint8_t index, uint8_t parameter)
{
   uint8_t row = index % 4;
   uint8_t x1 = VAR_NAME_WIDTH + BUTTON_PADDING;
   uint8_t y1 = PAGEINDICATOR_HEIGHT + BUTTON_PADDING + row * (BUTTON_HEIGHT + BUTTON_PADDING);
   tft.fillRect(x1 + 2, y1 + 2, VAR_VALUE_WIDTH - 4, BUTTON_HEIGHT - 4, ILI9341_BLACK);
   tft.drawRect(x1 + 1 , y1 + 1, VAR_VALUE_WIDTH - 2, BUTTON_HEIGHT - 2, ILI9341_BLUE);
   tft.setCursor(x1 + 8, y1 + 5);
   tft.setTextColor(ILI9341_WHITE);
   float value = getValue(parameter);
   if (displayParameters[parameter].enumFunction != nullptr) tft.print(displayParameters[parameter].enumFunction(value));
   else tft.print(value, displayParameters[parameter].displayPrecision);
}

void drawPotWidget(uint8_t index, uint8_t parameter, bool selected, bool drawStatics)
{
//  uint8_t row = index / 4;
//  uint8_t column = index % 4;
//  
//  uint16_t boxColor = ILI9341_DARKGREY;
//  if (selected) boxColor = ILI9341_BLUE;
//  const uint8_t boxWidth = 70;
//  const uint8_t boxHeight = 100;
//  const uint8_t boxPadding = 5;
//  const uint8_t radius = 25;
//
//  uint8_t xBox = boxPadding + column * (boxWidth + boxPadding);
//  uint8_t yBox = HEADER_HEIGHT + boxPadding + row * (boxHeight + boxPadding);
//  uint8_t xCenter = xBox +  boxWidth / 2;
//  uint8_t yCenter = yBox + boxHeight / 2;
//
//  const uint16_t textBoxColor = ILI9341_LIGHTGREY;
//  const uint8_t textBoxWidth = boxWidth - 8;
//  const uint8_t textBoxHeight = 16;
//  tft.setFont(Arial_10);
//  tft.setTextColor(ILI9341_BLACK);
//  uint8_t xNameBox = xBox + 4;
//  uint8_t yNameBox = yBox + 4;
//  uint8_t xValueBox = xBox + 4;
//  uint8_t yValueBox = yBox + boxHeight - 4 - textBoxHeight;
//
//  const float angleSpan = 5.0; // radians
//  const float angleStart = 5.5; // radians
//  float value = getValue(parameter);
//  float angleFractional = (value - displayParameters[parameter].minValue) / ( displayParameters[parameter].maxValue - displayParameters[parameter].minValue );
//  float angle = angleStart - angleFractional * angleSpan;
//  uint8_t xLineEnd = xCenter + (radius - 5) * sin(angle);
//  uint8_t yLineEnd = yCenter + (radius - 5) * cos(angle);
//
//  if (drawStatics)
//  {
//    tft.fillRect(xBox, yBox, boxWidth, boxHeight, boxColor);  
//    tft.fillRect(xNameBox, yNameBox, textBoxWidth, textBoxHeight, textBoxColor);
//    tft.setCursor(xNameBox + 2, yNameBox + 3);
//    tft.print(displayParameters[parameter].name);
//    tft.drawCircle(xCenter, yCenter, radius, ILI9341_WHITE);
//  }
//  
//  tft.fillRect(xValueBox, yValueBox, textBoxWidth, textBoxHeight, textBoxColor);
//  tft.setCursor(xValueBox + 3, yValueBox + 3);
//  tft.print(value, displayParameters[parameter].displayPrecision);
//  tft.fillCircle(xCenter, yCenter, radius - 4, boxColor);
//  tft.drawLine(xCenter, yCenter, xLineEnd, yLineEnd, ILI9341_WHITE);
}

void handleSpecialPages(bool firstCall)
{
  switch (pages[currentPage].pageType)
  {
    case PAGE_LISTDEVICES:
      displayDevicePage(firstCall);
      break;
  }
}

void displayDevicePage(bool firstCall)
{
  if (firstCall)
  {
    tft.fillRect(0, PAGEINDICATOR_HEIGHT, SCREEN_XRES - HEADER_WIDTH - 1, SCREEN_YRES - PAGEINDICATOR_HEIGHT , ILI9341_BLACK);
    for (uint8_t usbIndex = 0; usbIndex < 4; usbIndex++)
    {
      char buf[16];
      getUsbDeviceName(usbIndex, buf, 15);
      tft.setCursor(5, (usbIndex + 1) * (PAGEINDICATOR_HEIGHT + 5));
      tft.print("USB");
      tft.print(usbIndex);
      tft.print(": ");
      tft.print(buf);
    }
  }
  if (updateButton(1)) currentPage = pages[currentPage].pageBack;  
}


// LAUNCHPAD UI FUNCTIONS
// ********************************************

void LPinit()
{
  LP1.setPadColor(CCstartStop, LP_RED);
  LP1.setPadColor(CCeditMode, LP_RED);
  LP1.setPadColor(CCsongMode, LP_GREEN);
}

void updateLaunchpadUI()
{
  static uint8_t oldTrack = 255;
  if (currentTrack != oldTrack)
  {
    oldTrack = currentTrack;
    LP1.setActiveTrack(29 + currentTrack * 10 , tracks[currentTrack]->color);
    LPsetPageFromTrackData();
  }
  if (sequencerState == STATE_RUNNING) LPsetStepIndicator();
}

void LPsetStepIndicator()
{
  static uint8_t previousColumn = 0;
  uint16_t  ticksPerColumn = ticksPerBeat/LP1.xZoomLevel;
  uint8_t column = tracks[currentTrack]->getCurrentColumn(ticksPerColumn);
  // check if currentColumn is in range of current page
  if (column != previousColumn)
  {
    if ( (LP1.page * 8 <= column) && (column <= LP1.page * 8 + 7) )
    {
      // current column is in display range --> set current column
      LP1.setColumnColor(column - LP1.page * 8, LP_RED);
    }
    if ((LP1.page * 8 <= previousColumn) && (previousColumn <= LP1.page * 8 + 7))
    {
      // previous column is in display range --> reset previous column
      LPsetColumnFromTrackData(previousColumn - LP1.page  * 8);
    }
    previousColumn = column;
  }
}

void LPpageIncrease()
{
  uint8_t ticksPerColumn = ticksPerBeat / LP1.xZoomLevel;
  if ((LP1.page * 8 + 7) < (tracks[currentTrack]->getPatternLengthColumns(ticksPerColumn) - 1))
  {
    LP1.page = LP1.page + 1;
    LPsetPageFromTrackData();
  }
}

void LPpageDecrease()
{
  if (LP1.page > 0)
  {
    LP1.page = LP1.page - 1;
    LPsetPageFromTrackData();
  }
}

void LPscrollUp()
{
  if (tracks[currentTrack]->lowerRow + LP1.yScrollStep < 127) tracks[currentTrack]->lowerRow = tracks[currentTrack]->lowerRow + LP1.yScrollStep;
  else tracks[currentTrack]->lowerRow = 120;
  LPsetPageFromTrackData();
  Serial.printf("Lower row: %d\n", tracks[currentTrack]->lowerRow);
}

void LPscrollDown()
{
  if (tracks[currentTrack]->lowerRow >= LP1.yScrollStep) tracks[currentTrack]->lowerRow = tracks[currentTrack]->lowerRow - LP1.yScrollStep;
  else tracks[currentTrack]->lowerRow = 0;
  LPsetPageFromTrackData();
  Serial.printf("Lower row: %d\n", tracks[currentTrack]->lowerRow);
}

void LPsetPageFromTrackData()
{
  for (uint8_t padColumn = 0; padColumn < 8; padColumn++) { LPsetColumnFromTrackData(padColumn); }
}

void LPsetColumnFromTrackData(uint8_t padColumn)
{
  // This function considers which page is currently displayed so only call with column 0 - 7.
  uint8_t padStateArray[24];
  uint8_t ticksPerColumn = 24 / LP1.xZoomLevel;
  uint16_t tickTemp = (padColumn + LP1.page * 8) * ticksPerColumn;
  uint8_t lowerRow = tracks[currentTrack]->lowerRow;
  
  for (uint8_t row = 0; row < 8; row++)
  {
    uint8_t padState = LP_OFF;
    uint8_t padId = 11 + padColumn + row * 10;
    //note: pad row 0 represents note value (lowerRow)
   
    if ((row == 0) && (lowerRow > 0) && (tracks[currentTrack]->getEventsInTickNoteInterval(tickTemp, tickTemp + ticksPerColumn - 1, 0, lowerRow - 1) > 0)) padState = LP_GHOST;         // notes below current column
    if ((row == 7) && (lowerRow < 120) && (tracks[currentTrack]->getEventsInTickNoteInterval(tickTemp, tickTemp + ticksPerColumn - 1, lowerRow + 7 + 1, 127) > 0)) padState = LP_GHOST; // notes above current column
    if (tracks[currentTrack]->getEventsInTickInterval(tickTemp, tickTemp + ticksPerColumn - 1 , lowerRow + row) > 0) padState = tracks[currentTrack]->color;
    padStateArray[3 * row] = 0; // solid color
    padStateArray[3 * row + 1] = padId;
    padStateArray[3 * row + 2] = padState;
  }
  LP1.setMultiplePadColorState(padStateArray, 24);
}

void setLPsongMode()
{
  uint8_t padStateArray[6] = {0, CCsongMode, LP_GREEN, 0, CCpatternMode, LP_OFF};
  LP1.setMultiplePadColorState(padStateArray, 6);
  LPdisplayMode = LPMODE_SONG;
}

void setLPpatternMode()
{
  uint8_t padStateArray[6] = {0, CCsongMode, LP_OFF, 0, CCpatternMode, LP_GREEN};
  LP1.setMultiplePadColorState(padStateArray, 6);
  LPdisplayMode = LPMODE_PATTERN;
}

void LPdisplayPatternPage(bool firstCall)
{
  
  
}

uint16_t lpColor2tftColor(uint8_t lpColor)
{
  switch (lpColor)
  {
    case LP_RED: return TFT_RED;
    case LP_BLUE: return TFT_BLUE;
    case LP_GREEN: return TFT_GREEN;
    case LP_PURPLE: return TFT_PURPLE;
    case LP_CYAN: return TFT_CYAN;
    case LP_ORANGE: return TFT_ORANGE;
    case LP_PINK: return TFT_PINK;
    case LP_GHOST: return TFT_GHOST;
    case LP_DARKBLUE: return TFT_DARKBLUE;
    default: return 0;
  }
}

// HARDWARE CONTROLS
// ********************************************
int16_t getEncoderDirection(uint8_t encoderNr)
{
  int16_t value = encoders[encoderNr].read();
  int16_t returnValue = 0;
  if (value > 3)
  {
    returnValue = 1;
    encoders[encoderNr].write(0);
    //Serial.printf("Encoder %d: %d \n", encoderNr, value);
  }
  if (value < -3)
  {
    returnValue = -1;
    encoders[encoderNr].write(0);
    //Serial.printf("Encoder %d: %d \n", encoderNr, value);
  }
  return returnValue;
}

uint8_t updateButton(uint8_t buttonNr)
{
  bouncers[buttonNr].update();
  if (bouncers[buttonNr].fallingEdge())
  {
    //Serial.print("Button ");
    //Serial.println(buttonNr);
    return 1;
  }
  else return 0;
}

void initMcp()
{
  Wire1.begin();
  mcp.init();
  mcp.portMode(MCP23017Port::A, 0);          //Port A as output
  mcp.portMode(MCP23017Port::B, 0b11111111); //Port B as input

  mcp.writeRegister(MCP23017Register::GPPU_B, 0xFF);  //port B weak pullup
  mcp.writeRegister(MCP23017Register::IPOL_B, 0xFF);  //port B inverted polarity
  
  mcp.writeRegister(MCP23017Register::GPIO_A, 0x00);  //Reset port A 
  mcp.writeRegister(MCP23017Register::GPIO_B, 0x00);  //Reset port B
}

void updateMcp()
{
  static uint8_t portB = 0;
  uint8_t currentB = mcp.readPort(MCP23017Port::B); // read buttons, pressed = 1
  if (portB != currentB)
  {
    //Serial.printf("MCP: %d\n", currentB);
    portB = currentB;
    switch (portB)
    {
      case 8:
        currentPage = PAGE_SONG;
        //mcp.writePort(MCP23017Port::A, 8); // write leds, 1 = led lit
        break;
      case 4:
        currentPage = PAGE_PATTERN;
        //mcp.writePort(MCP23017Port::A, 4);
        break;
      case 2:
        currentPage = PAGE_EVENT;
        //mcp.writePort(MCP23017Port::A, 2);
        break;
      case 1:
        currentPage = PAGE_TRACK;
        //mcp.writePort(MCP23017Port::A, 1);
        break;
      case 16:
        currentPage = PAGE_FILE;
        //mcp.writePort(MCP23017Port::A, 16);
        break;
       case 128:
        currentPage = PAGE_LISTDEVICES;
        //mcp.writePort(MCP23017Port::A, 128);
        break;
        
    }
  }
}
