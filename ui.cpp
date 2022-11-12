#include "ui.h"

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
Encoder encoders[2] = {Encoder(ROT1_A, ROT1_B), Encoder(ROT2_A, ROT2_B)};
Bounce bouncers[2] = {Bounce(ROT1_SW, 5), Bounce(ROT2_SW, 5)}; 
MCP23017 mcp = MCP23017(MCP23017_ADDR, Wire1);

uint8_t currentTrack = 0;
uint8_t currentPattern = 0;
uint8_t currentPage = PAGE_HOME;
uint8_t LPdisplayMode = DISPLAYMODE_SEQUENCER;

//                      .pageType .name       .pageBack   .nrChildren  .children  .widget
const page pages[] = {
                      {PAGE_NAV,  "HOME",     PAGE_HOME,  6,         {PAGE_SEQ, PAGE_TRACK, PAGE_VOICE, PAGE_EFX, PAGE_MIX, PAGE_LISTDEVICES}, W_NONE},
                      {PAGE_NAV,  "VOICE",    PAGE_HOME,  2,         {PAGE_OSC_WFM, PAGE_OSC_PITCH}, W_NONE},
                      {PAGE_PAR,  "AMPENV",   PAGE_VOICE, 4,         {AMPENV_ATTACK, AMPENV_DECAY, AMPENV_SUSTAIN, AMPENV_RELEASE}, W_POT},
                      {PAGE_PAR,  "FLTENV",   PAGE_VOICE, 4,         {FILTERENV_ATTACK, FILTERENV_DECAY, FILTERENV_SUSTAIN, FILTERENV_RELEASE}, W_POT},
                      {PAGE_NAV,  "MOD",      PAGE_VOICE, 2,         {PAGE_OSC_WFM, PAGE_OSC_PITCH}, W_NONE},
                      {PAGE_NAV,  "EFX",      PAGE_HOME,  2,         {PAGE_CHORUS, PAGE_REVERB}, W_NONE},
                      {PAGE_PAR,  "MIX",      PAGE_HOME,  3,         {MIX_DRY, MIX_CHORUS, MIX_REVERB}, W_POT},
                      {PAGE_PAR,  "OSCWF",    PAGE_VOICE, 2,         {OSC1WFM, OSC2WFM}, W_LIST},
                      {PAGE_PAR,  "OSCTN",    PAGE_VOICE, 2,         {OSC2TRANSPOSE, OSC_DETUNE}, W_POT},
                      {PAGE_PAR,  "CHORUS",   PAGE_EFX,   1,         {CHORUS_LFORATE}, W_POT},
                      {PAGE_PAR,  "REVERB",   PAGE_EFX,   4,         {REVERB_SIZE, REVERB_LODAMP, REVERB_HIDAMP, REVERB_DIFFUSION}, W_POT},
                      {PAGE_PAR,  "TRACK",    PAGE_HOME,  4,         {TRACK_LENGTH, TRACK_SPEED, TRACK_CHANNEL, TRACK_OUTPUT}, W_LIST},
                      {PAGE_PAR,  "SEQ",      PAGE_HOME,  1,         {SEQ_BPM}, W_LIST},
                      {PAGE_LISTDEVICES,  "USBDV",      PAGE_HOME,  1,         {}, W_LIST}
               
};

//                                        name        .value  .minValue .maxValue .multiplier .displayPrecision  .getFunction .setFunction  .enumFunction                                          
const parameters displayParameters[] = { 
                                         {"OSC1WF",   0,      0,        3,        1.0,        0,    nullptr, nullptr, nullptr},
                                         {"OSC2WF",   0,      0,        3,        1.0,        0,    nullptr, nullptr, nullptr},
                                         {"OSC2TRP",  0,    -24,       24,        1.0,        0,    nullptr, nullptr, nullptr},
                                         {"OSC DET",  0,      0,        1,        0.01,       2,    nullptr, nullptr, nullptr},
                                         {"ATTACK",   0,      0,        500.0,    1.0,        0,    nullptr, nullptr, nullptr},
                                         {"DECAY",    0,      0,        500.0,    10.0,       0,    nullptr, nullptr, nullptr},
                                         {"SUSTAIN",  0,      0,        1.0,      0.01,       2,    nullptr, nullptr, nullptr},
                                         {"RELEASE",  0,      0,        3000.0,   10.0,       0,    nullptr, nullptr, nullptr},
                                         {"ATTACK",   0,      0,        500.0,    1.0,        0,    nullptr, nullptr, nullptr},
                                         {"DECAY",    0,      0,        500.0,    10.0,       0,    nullptr, nullptr, nullptr},
                                         {"SUSTAIN",  0,      0,        1.0,      0.01,       2,    nullptr, nullptr, nullptr},
                                         {"RELEASE",  0,      0,        3000.0,   10.0,       0,    nullptr, nullptr, nullptr},
                                         {"LFORATE",  0,      0,        12,       0.1,        1,    nullptr, nullptr, nullptr},
                                         {"SIZE",     0,      0,        1,        0.1,        1,    nullptr, nullptr, nullptr},
                                         {"LODAMP",   0,      0,        1,        0.1,        1,    nullptr, nullptr, nullptr},
                                         {"HIDAMP",   0,      0,        1,        0.1,        1,    nullptr, nullptr, nullptr},
                                         {"DIFFUS",   0,      0,        1,        0.1,        1,    nullptr, nullptr, nullptr},
                                         {"DRY",      0,      0,        2.0,      0.1,        1,    &getDryLevel, &setDryLevel, nullptr},
                                         {"CHORUS",   0,      0,        2.0,      0.1,        1,    &getChorusLevel, &setChorusLevel, nullptr},
                                         {"REVERB",   0,      0,        2.0,      0.1,        1,    &getReverbLevel, &setReverbLevel, nullptr},
                                         {"LENGTH",   0,      1,        64,        1,         0,    nullptr, nullptr, nullptr},
                                         {"SPEED",    0,      0,        64,        1,         0,    nullptr, nullptr, nullptr},
                                         {"OUTPUT",   0,      0,        6,         1,         0,    &getTrackOutput, &setTrackOutput, &getOutputEnum},
                                         {"BPM",      0,      20,       300,       1,         0,    &getBpm, &setBpm, nullptr},
                                         {"CHANNEL",  0,      0,        16,        1,         0,    &getTrackChannel, &setTrackChannel, nullptr}
                                      
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
  static uint8_t oldPage = PAGE_HOME;
  static bool firstCall = true;
  if (pages[currentPage].pageType == PAGE_NAV) updateNavigationPage(pages[currentPage].children, pages[currentPage].nrChildren, firstCall);
  else if (pages[currentPage].pageType == PAGE_PAR) updateParameterPage(pages[currentPage].children, pages[currentPage].nrChildren, firstCall);
  else handleSpecialPages(firstCall);
  firstCall = !(oldPage == currentPage);
  oldPage =  currentPage;
  updateHeader(currentTrack, currentPattern, 120, false);
}

void updateHeader(uint8_t trackNr, uint8_t patternNr, uint8_t bpm, bool firstCall)
{
  static uint8_t oldTrackNr = 255;
  static uint8_t oldPatternNr = 255;
  static uint8_t oldBpm = 255;
  static uint8_t oldPage = 255;
  const uint16_t xTrack = 85;
  const uint16_t xPattern = 150;
  const uint16_t xBpm = 230;
  
  if (firstCall)
  {
    tft.fillRect(85, 0, SCREEN_XRES - 85, HEADER_HEIGHT, ILI9341_LIGHTGREY);
    tft.setTextColor(ILI9341_BLACK);
    tft.setFont(Arial_14);
    tft.setCursor(xTrack + 5, 6);
    tft.print("TRK");
    tft.setCursor(xPattern + 5, 6);
    tft.print("PTN");
    tft.setCursor(xBpm + 5, 6);
    tft.print("BPM");
  }
  if (oldPage != currentPage)
  {
    oldPage = currentPage;
    tft.fillRect(0, 0, 80, HEADER_HEIGHT, ILI9341_BLUE);
    tft.setCursor(3, 6);
    tft.setFont(Arial_14);
    tft.setTextColor(ILI9341_WHITE);
    tft.print(pages[currentPage].name);
  }
  if (oldTrackNr != trackNr)
  {
    tft.fillRect(xTrack + 50, 0, 20, HEADER_HEIGHT, ILI9341_LIGHTGREY);
    tft.setFont(Arial_14);
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(xTrack + 50, 6);
    tft.print(trackNr);
    oldTrackNr = trackNr;
  }
  if (oldPatternNr != patternNr)
  {
    tft.fillRect(xPattern + 50, 0, 30, HEADER_HEIGHT, ILI9341_LIGHTGREY);
    tft.setFont(Arial_14);
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(xPattern + 50, 6);
    tft.printf("%02d", patternNr);
    oldPatternNr = patternNr;
  }
  if (oldBpm != bpm)
  {
    tft.fillRect(xBpm + 50, 0, 30, HEADER_HEIGHT, ILI9341_LIGHTGREY);
    tft.setFont(Arial_14);
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(xBpm + 50, 6);
    tft.printf("%03d", bpm);
    oldBpm = bpm;
  }
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
  uint8_t x1 = BUTTON_PADDING + column * (BUTTON_WIDTH + BUTTON_PADDING);
  uint8_t y1 = HEADER_HEIGHT + BUTTON_PADDING + row * (BUTTON_HEIGHT + BUTTON_PADDING);
  const uint8_t w = BUTTON_WIDTH;
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
  tft.setFont(Arial_14);
  tft.setCursor(x1 + 8, y1 + 5);
  tft.print(text);
}


void updateParameterPage(const uint8_t * parameterArray, const uint8_t nrParameters, bool firstCall)
{
  static int16_t selectedButton = 0;
  float variables[nrParameters];
  for (uint8_t i = 0; i < nrParameters; i++) variables[i] = getValue(parameterArray[i]);
 
  if (firstCall)
  {
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_XRES, SCREEN_XRES - HEADER_HEIGHT , ILI9341_BLACK);
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
   uint8_t x1 = 2 * BUTTON_PADDING + BUTTON_WIDTH;
   uint8_t y1 = HEADER_HEIGHT + BUTTON_PADDING + row * (BUTTON_HEIGHT + BUTTON_PADDING);
   tft.fillRect(x1 + 1, y1 + 1, BUTTON_WIDTH - 2, BUTTON_HEIGHT - 2, ILI9341_BLACK);
   tft.drawRect(x1 + 1 , y1 + 1, BUTTON_WIDTH - 2, BUTTON_HEIGHT - 2, ILI9341_BLUE);
   tft.setCursor(x1 + 8, y1 + 5);
   tft.setTextColor(ILI9341_WHITE);
   float value = getValue(parameter);
   if (displayParameters[parameter].enumFunction != nullptr) tft.print(displayParameters[parameter].enumFunction(value));
   else tft.print(value, displayParameters[parameter].displayPrecision);
}

void drawPotWidget(uint8_t index, uint8_t parameter, bool selected, bool drawStatics)
{
  uint8_t row = index / 4;
  uint8_t column = index % 4;
  
  uint16_t boxColor = ILI9341_DARKGREY;
  if (selected) boxColor = ILI9341_BLUE;
  const uint8_t boxWidth = 70;
  const uint8_t boxHeight = 100;
  const uint8_t boxPadding = 5;
  const uint8_t radius = 25;

  uint8_t xBox = boxPadding + column * (boxWidth + boxPadding);
  uint8_t yBox = HEADER_HEIGHT + boxPadding + row * (boxHeight + boxPadding);
  uint8_t xCenter = xBox +  boxWidth / 2;
  uint8_t yCenter = yBox + boxHeight / 2;

  const uint16_t textBoxColor = ILI9341_LIGHTGREY;
  const uint8_t textBoxWidth = boxWidth - 8;
  const uint8_t textBoxHeight = 16;
  tft.setFont(Arial_10);
  tft.setTextColor(ILI9341_BLACK);
  uint8_t xNameBox = xBox + 4;
  uint8_t yNameBox = yBox + 4;
  uint8_t xValueBox = xBox + 4;
  uint8_t yValueBox = yBox + boxHeight - 4 - textBoxHeight;

  const float angleSpan = 5.0; // radians
  const float angleStart = 5.5; // radians
  float value = getValue(parameter);
  float angleFractional = (value - displayParameters[parameter].minValue) / ( displayParameters[parameter].maxValue - displayParameters[parameter].minValue );
  float angle = angleStart - angleFractional * angleSpan;
  uint8_t xLineEnd = xCenter + (radius - 5) * sin(angle);
  uint8_t yLineEnd = yCenter + (radius - 5) * cos(angle);

  if (drawStatics)
  {
    tft.fillRect(xBox, yBox, boxWidth, boxHeight, boxColor);  
    tft.fillRect(xNameBox, yNameBox, textBoxWidth, textBoxHeight, textBoxColor);
    tft.setCursor(xNameBox + 2, yNameBox + 3);
    tft.print(displayParameters[parameter].name);
    tft.drawCircle(xCenter, yCenter, radius, ILI9341_WHITE);
  }
  
  tft.fillRect(xValueBox, yValueBox, textBoxWidth, textBoxHeight, textBoxColor);
  tft.setCursor(xValueBox + 3, yValueBox + 3);
  tft.print(value, displayParameters[parameter].displayPrecision);
  tft.fillCircle(xCenter, yCenter, radius - 4, boxColor);
  tft.drawLine(xCenter, yCenter, xLineEnd, yLineEnd, ILI9341_WHITE);
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
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_XRES, SCREEN_XRES - HEADER_HEIGHT , ILI9341_BLACK);
    for (uint8_t usbIndex = 0; usbIndex < 4; usbIndex++)
    {
      char buf[13];
      getUsbDeviceName(usbIndex, buf, 12);
      drawMenuButton(usbIndex ,usbIndex , false);
      drawMenuButton(usbIndex + 5 ,buf, false);
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

void LPdisplayPatternPage(bool firstCall)
{
  
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
    Serial.printf("MCP: %d\n", currentB);
    portB = currentB;
    switch (portB)
    {
      case 1:
        currentPage = PAGE_HOME;
        mcp.writePort(MCP23017Port::A, ~1); // write leds, 0 = led lit
        break;
      case 2:
        currentPage = PAGE_MIX;
        mcp.writePort(MCP23017Port::A, ~2);
        break;
      case 4:
        currentPage = PAGE_LISTDEVICES;
        mcp.writePort(MCP23017Port::A, ~4);
        break;
      case 8:
        break;
      case 16:
        break;
        
    }
  }
}
