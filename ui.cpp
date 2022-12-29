#include "ui.h"

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
Encoder encoders[2] = {Encoder(ROT1_A, ROT1_B), Encoder(ROT2_A, ROT2_B)};
Bounce bouncers[2] = {Bounce(ROT1_SW, 5), Bounce(ROT2_SW, 5)}; 
MCP23017 mcp = MCP23017(MCP23017_ADDR, Wire1);


uint8_t currentPage = PAGE_SONG;
uint8_t LPdisplayMode = LPMODE_PATTERN;

//                      .pageType    .name       .nrChildren  .children
const page pages[] = {
                      {PAGE_PAR,      "SONG",     3,          {SONG_BPM, SONG_PLAYMODE, MIDI_CLOCK}},
                      {PAGE_PAR,      "PATTERN",  4,          {PATTERN_NR, PATTERN_LENGTH, PATTERN_SPEED, PATTERN_EVENTLENGTHDEF}},
                      {PAGE_PAR,      "EVENT",    1,          {EVENT_LENGTH}},
                      {PAGE_PAR,      "TRACK",    3,          {TRACK_CHANNEL, TRACK_OUTPUT, TRACK_TRANSPOSESTATUS}},
                      {PAGE_FILE,     "FILE",     0,          {}},
                      {PAGE_LISTDEVICES,  "USBDEV",    1,     {}},
                      {PAGE_PAR,      "SCENE",    2,          {SCENE_NR, SCENE_COLOR}},
                      {PAGE_TOOLS,    "TOOLS",    3,          {TOOLS_SELECTION, TOOLS_ACTION, TOOLS_TRANSPOSE}}
               
};

//                                        name        .minValue .maxValue .multiplier .displayPrecision  .getFunction .setFunction  .enumFunction                                          
const parameters displayParameters[] = { 
                                         {"LENGTH",   1,        128,       1,         0,    &getTrackLengthColumns, &setTrackLengthColumns, nullptr},
                                         {"SPEED",    0,        4,         1,         0,    &getPatternSpeed, &setPatternSpeed, &getPatternSpeedEnum},
                                         {"DEF.LEN",  0,        64,        1,         0,    &getTrackDefLength, &setTrackDefLength, nullptr},
                                         {"OUTPUT",   0,        7,         1,         0,    &getTrackOutput, &setTrackOutput, &getOutputEnum},
                                         {"BPM",      20,       300,       1,         0,    &getBpm, &setBpm, nullptr},
                                         {"CHANNEL",  0,        17,        1,         0,    &getTrackChannel, &setTrackChannel, &get0_16_note},
                                         {"LENGTH",   1,        128,       1,         0,    &getEventLength, &setEventLength, nullptr},
                                         {"REC.TRP",  0,        2,         1,         0,    &getTransposeStatus, &setTransposeStatus, &getNoYesSelectedEnum},
                                         {"NR",       0,        7,         1,         0,    &getTrackPatternNr, &setTrackPatternNr, nullptr},
                                         {"NR",       0,        15,        1,         0,    &getSceneNr, &setSceneNr, nullptr},
                                         {"COLOR",    0,        127,       1,         0,    &getSceneColor, &setSceneColor, nullptr},

                                         {"SELECTN",  0,        2,         1,         0,    &getToolSelection, &setToolSelection, getSelectionEnum},
                                         {"ACTION",   0,        2,         1,         0,    &getToolAction, &setToolAction, getActionEnum},
                                         {"TRANSP",   -24,      24,        1,         0,    &getToolTranspose, &setToolTranspose, nullptr},
                                         {"MIDI CLK", 0,        1,         1,         0,    &getMidiClockOnOff, &setMidiClockOnOff, &getNoYesSelectedEnum},
                                         {"CHAIN",    0,        1,         1,         0,    &getPlaymode, &setPlaymode, &getNoYesSelectedEnum}
                                        };

                                        
#define NR_DISPLAYPARAMETERS sizeof(displayParameters) / sizeof(displayParameters[0])
float displayedParameterValues[NR_DISPLAYPARAMETERS];

void setupUI()
{
  Serial.println("UI SETUP");
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(MAIN_BG_COLOR);
  showStartupScreen();
  pinMode(ROT1_SW, INPUT_PULLUP);
  pinMode(ROT2_SW, INPUT_PULLUP);
  initMcp();
  updateHeader(currentTrack, currentPattern, 120, true);
}

void showStartupScreen()
{
  tft.setFont(Arial_24);
  tft.setTextColor(ILI9341_BLUE);
  tft.setCursor(20, 88);
  tft.print(F("BEATMAST3R 2K"));
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(22, 90);
  tft.print(F("BEATMAST3R 2K"));
  for (uint8_t i = 0; i < 16; i++)
  {
    tft.drawFastHLine(10, 80 - i * 4, 300, tft.color565(0, 0, 200 - i * 10));
    tft.drawFastHLine(10, 120 + i * 4, 300, tft.color565(0, 0, 200 - i * 10));
    delay(200);
  }
  tft.fillScreen(MAIN_BG_COLOR);
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
  if (pages[currentPage].pageType == PAGE_PAR) updateParameterPage(pages[currentPage].children, pages[currentPage].nrChildren, firstCall, forceVariableUpdate);
  else handleSpecialPages(firstCall);
  firstCall = !(oldPage == currentPage);
  oldPage =  currentPage;
  forceVariableUpdate = updateHeader(currentTrack, currentPattern, SequencerData.bpm, false);
  LPcopy_update(firstCall, forceVariableUpdate);
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
  }
  if (oldPage != currentPage)
  {
    oldPage = currentPage;
    tft.fillRect(0, 0, PAGEINDICATOR_WIDTH, PAGEINDICATOR_HEIGHT, PAGEINDICATOR_COLOR);
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
  if ( (oldLPpage != LP1.page) || (oldTrackColumns !=  tracks[currentTrack]->getPatternLengthColumns(TICKS_PER_COLUMN)))
  {
    uint8_t trackColumns = tracks[currentTrack]->getPatternLengthColumns(TICKS_PER_COLUMN);
    oldLPpage = LP1.page;
    oldTrackColumns = trackColumns;
  }
  return false;
}

void drawMenuButton(uint8_t index, String text, bool selected)
{
  uint8_t row = index % 5;
  uint8_t column = index / 5 ;
  uint8_t x1 = BUTTON_PADDING + column * (VAR_NAME_WIDTH + BUTTON_PADDING);
  uint8_t y1 = PAGEINDICATOR_HEIGHT + BUTTON_PADDING + row * (BUTTON_HEIGHT + BUTTON_PADDING);
  const uint8_t w = VAR_NAME_WIDTH;
  const uint8_t h = BUTTON_HEIGHT;
  const uint8_t textYpadding = 6;
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
  tft.setCursor(x1 + 8, y1 + textYpadding);
  tft.print(text);
}

void updateParameterPage(const uint8_t * parameterArray, const uint8_t nrParameters, bool firstCall, bool forceVariableUpdate)
{
  static int16_t selectedButton = 0;
  
  // draw statics on first call only
  if (firstCall)
  {
    tft.fillRect(0, PAGEINDICATOR_HEIGHT, HEADER_X - 1, NAV_AREA_HEIGHT , MAIN_BG_COLOR);
    selectedButton = 0;
    encoders[0].write(selectedButton);
    for (uint8_t i = 0; i < nrParameters; i++) drawParameterRow(i, parameterArray[i], i == 0);
  }
  
  // update value if changed only
  for (uint8_t i = 0; i < nrParameters; i++)
  {
    float currentValue = getValue(parameterArray[i]);
    float displayedValue = displayedParameterValues[parameterArray[i]];
    if (displayedValue != currentValue)
    {
      displayedParameterValues[parameterArray[i]] = currentValue;
      updateParameterRow(i, parameterArray[i]);
    }
  }

  // select row with encoder 0
  int16_t newSelectedButton = selectedButton + getEncoderDirection(0);
  if (newSelectedButton < 0) newSelectedButton = nrParameters - 1;
  if (newSelectedButton > nrParameters - 1) newSelectedButton = 0;
  if (newSelectedButton != selectedButton)
  {
    drawParameterRow(selectedButton, parameterArray[selectedButton], false);
    selectedButton = newSelectedButton;
    drawParameterRow(selectedButton, parameterArray[selectedButton], true);
    encoders[1].write(0);
  }

  // update value with encoder 1
  int16_t deltaBaseValue = encoders[1].read() / 4;
  float newValue = 0.0;
  if (abs(deltaBaseValue) > 0)
  {
    encoders[1].write(0);
    newValue = constrain(displayedParameterValues[parameterArray[selectedButton]] + deltaBaseValue * displayParameters[parameterArray[selectedButton]].multiplier, displayParameters[parameterArray[selectedButton]].minValue, displayParameters[parameterArray[selectedButton]].maxValue);
    updateValue(parameterArray[selectedButton], newValue);
    updateParameterRow(selectedButton, parameterArray[selectedButton]);
  }
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
   const uint8_t textYpadding = 6;
   tft.fillRect(x1 + 2, y1 + 2, VAR_VALUE_WIDTH - 4, BUTTON_HEIGHT - 4, MAIN_BG_COLOR);
   tft.drawRect(x1 + 1 , y1 + 1, VAR_VALUE_WIDTH - 2, BUTTON_HEIGHT - 2, ILI9341_BLUE);
   tft.setCursor(x1 + 8, y1 + textYpadding);
   tft.setTextColor(ILI9341_WHITE);
   tft.setFont(Arial_12);
   float value = getValue(parameter);
   if (displayParameters[parameter].enumFunction != nullptr) tft.print(displayParameters[parameter].enumFunction(value));
   else tft.print(value, displayParameters[parameter].displayPrecision);
}

void handleSpecialPages(bool firstCall)
{
  switch (pages[currentPage].pageType)
  {
    case PAGE_LISTDEVICES:
      displayDevicePage(firstCall);
      break;
    case PAGE_FILE:
      displayFilePage(firstCall);
      break;
    case PAGE_TOOLS:
      displayToolsPage(firstCall);
      break;
  }
}

void displayDevicePage(bool firstCall)
{
  if (firstCall)
  {
    tft.fillRect(0, PAGEINDICATOR_HEIGHT, SCREEN_XRES - HEADER_WIDTH - 1, SCREEN_YRES - PAGEINDICATOR_HEIGHT , MAIN_BG_COLOR);
    for (uint8_t usbIndex = 0; usbIndex < 5; usbIndex++)
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
}

void displayFilePage(bool firstCall)
{
  static uint8_t fileNr = 0;
  if (firstCall)
  {
    tft.fillRect(0, PAGEINDICATOR_HEIGHT, SCREEN_XRES - HEADER_WIDTH - 1, 160 , MAIN_BG_COLOR);
    displayFileNr(fileNr);
    displayLoadSave();
  }
  int newFileNr = fileNr + getEncoderDirection(0);
  if (newFileNr > MAX_FILES - 1) newFileNr = 0;
  if (newFileNr < 0) newFileNr = MAX_FILES - 1;
  if ((uint8_t)newFileNr != fileNr)
  {
    fileNr = (uint8_t)newFileNr;
    displayFileNr(fileNr);
  }
  if (updateButton(0))
  {
    loadFile(fileNr);
    if (LPdisplayMode == LPMODE_PATTERN) LPsetPageFromTrackData();
    if (LPdisplayMode == LPMODE_SCENE)
    {
      setScene(0);
      LPsetSceneButtonsSceneMode(true);
      LPsetPageFromSceneData(true);
    }
    LPcopy_update(true, true);
    setBpm(SequencerData.bpm);
  }
  if (updateButton(1)) saveFile(fileNr);
}

void displayFileNr(uint8_t fileNr)
{
   tft.fillRect(10, PAGEINDICATOR_HEIGHT + 30, 100, 19 , MAIN_BG_COLOR);
   tft.setCursor(10, PAGEINDICATOR_HEIGHT + 30);
   tft.setFont(Arial_18);
   tft.setTextColor(ILI9341_WHITE);
   tft.printf("ID: %02d", fileNr);
   tft.setFont(Arial_12);
}

void displayLoadSave()
{
  displayEncoderButtonHint(0, "LOAD", TFT_GREEN);
  displayEncoderButtonHint(1, "SAVE", TFT_RED);
}

void displayEncoderButtonHint(uint8_t encoder, String hint, uint16_t color)
{
  const uint8_t radius = 25;
  const uint8_t y = 160;
  const uint8_t x = 60 +  encoder * (radius + 100);

  tft.setTextColor(ILI9341_BLACK);
  tft.setFont(Arial_12);
  tft.fillCircle(x, y, radius, color);
  tft.setCursor(x - radius + 5, y - 5);
  tft.print(hint);
  tft.setTextColor(ILI9341_WHITE);
}

void displayToolsPage(bool firstCall)
{
  updateParameterPage(pages[PAGE_TOOLS].children, pages[PAGE_TOOLS].nrChildren, firstCall, false);
  if (firstCall) displayEncoderButtonHint(0, "EXEC", TFT_RED);
  if (updateButton(0)) doToolAction();
}


// PATTERN COPY DISPLAY FUNCTIONS
// ******************************************** 

void LPcopy_update(bool firstCall, bool forceVariableUpdate)
{
  static uint16_t oldTrackColumns = 255;
  static uint8_t oldPattern = 255;
  static uint8_t oldTrack = 255;
  static uint8_t oldPage = 0;
  static uint8_t oldLowerRow = 0;
  //uint8_t ticksPerColumn = 24 / LP1.xZoomLevel;
  uint8_t trackColumns = tracks[currentTrack]->getPatternLengthColumns(TICKS_PER_COLUMN);
  uint8_t maxPages = trackColumns/8 + !!(trackColumns%8);
  if (firstCall)
  {
    LPcopy_clearArea();
    LPcopy_drawBackground(maxPages);
    LPcopy_updateAllEvents();
    oldTrack = currentTrack;
    oldTrackColumns = trackColumns;
    oldPattern = tracks[currentTrack]->getActivePatternId();
    oldLowerRow = tracks[currentTrack]->lowerRow;
  }
  if (LP1.page != oldPage)
  {
    oldPage = LP1.page;
    LPcopy_setSelectedPage();
  }
  if (oldTrackColumns !=  trackColumns || oldPattern != tracks[currentTrack]->getActivePatternId() || oldTrack != currentTrack || oldLowerRow != tracks[currentTrack]->lowerRow)
  {
    // IF nr columns changed redraw all (could be last parts only)
    // IF track or pattern changed redraw all
    LPcopy_clearArea();
    LPcopy_drawBackground(maxPages);
    LPcopy_updateAllEvents();
    oldTrack = currentTrack;
    oldTrackColumns = trackColumns;
    oldPattern = tracks[currentTrack]->getActivePatternId();
    oldLowerRow = tracks[currentTrack]->lowerRow;
    LPcopy_setSelectedPage();
  }
  if (sequencerState == STATE_RUNNING) LPcopy_setStepIndicator();
}

void LPcopy_clearArea() { tft.fillRect(LPCOPY_XPOS, LPCOPY_YPOS, HEADER_X - 10, 40, MAIN_BG_COLOR); }

void LPcopy_drawBackground(uint8_t nrPages)
{
  for (uint8_t page = 0; page < nrPages; page++)
  {
    for (uint8_t column = 0; column < 8; column++)
    {
      for (uint8_t row = 0; row < 8; row++)
      {
        tft.drawRect(LPCOPY_XPOS + page * (LPCOPY_BOXDIM * 8 + 5) + column * LPCOPY_BOXDIM, LPCOPY_YPOS + row * LPCOPY_BOXDIM, LPCOPY_BOXDIM, LPCOPY_BOXDIM, LPCOPY_BOX_OUTLINE_COLOR);
      }
    }
    if (LP1.page == page) tft.drawRect(LPCOPY_XPOS + page * (LPCOPY_BOXDIM * 8 + 5),LPCOPY_YPOS, 8 * LPCOPY_BOXDIM, 8 * LPCOPY_BOXDIM, LPCOPY_BOX_SELECTED_COLOR);
  }
}

void LPcopy_setSelectedPage()
{
  //uint8_t ticksPerColumn = 24 / LP1.xZoomLevel;
  uint8_t trackColumns = tracks[currentTrack]->getPatternLengthColumns(TICKS_PER_COLUMN);
  uint8_t maxPages = trackColumns/8 + !!(trackColumns%8);
  for (uint8_t page = 0; page < maxPages; page++)
  {
    uint16_t color = LPCOPY_BOX_OUTLINE_COLOR;
    if (LP1.page == page) color = LPCOPY_BOX_SELECTED_COLOR;
    tft.drawRect(LPCOPY_XPOS + page * (LPCOPY_BOXDIM * 8 + 5),LPCOPY_YPOS, 8 * LPCOPY_BOXDIM, 8 * LPCOPY_BOXDIM, color);
  }
}

void LPcopy_updateAllEvents()
{
  //uint8_t ticksPerColumn = 24 / LP1.xZoomLevel;
  uint8_t maxTrackColumns = tracks[currentTrack]->getPatternLengthColumns(TICKS_PER_COLUMN);
  for (uint8_t trackColumn = 0; trackColumn < maxTrackColumns; trackColumn++) { LPcopy_updateColumn(trackColumn); }
}

void LPcopy_updateColumn(uint8_t trackColumn)
{
  uint8_t padState = LP_OFF;
  //uint8_t ticksPerColumn = 24 / LP1.xZoomLevel;
  uint8_t page =  trackColumn/8;// + !!(trackColumn%8);
  uint16_t tickTemp = trackColumn * TICKS_PER_COLUMN;
  uint8_t lowerRow = tracks[currentTrack]->lowerRow;
  for (uint8_t row = 0; row < 8; row++)
  {
    padState = LP_OFF;
    if (tracks[currentTrack]->getEventsInTickInterval(tickTemp, tickTemp + TICKS_PER_COLUMN - 1 , lowerRow + row) > 0) padState = tracks[currentTrack]->color;
    LPcopy_updateSingleEvent(page, (trackColumn - page * 8), row, padState);
  }
}

void LPcopy_updateSingleEvent_converter(uint8_t note, uint8_t padState)
{
  LP1.setPadColor(note, padState);
  uint8_t column = LPnoteToPadColumn(note);
  uint8_t row = LPnoteToPadRow(note);
  LPcopy_updateSingleEvent(LP1.page, column, row, padState);
}

void LPcopy_updateSingleEvent(uint8_t page, uint8_t column, uint8_t row, uint8_t LPcolor)
{
  uint8_t xPos = LPCOPY_XPOS + page * (LPCOPY_BOXDIM * 8 + 5) + column * LPCOPY_BOXDIM + 1;
  uint8_t yPos = LPCOPY_YPOS + (7 - row) * LPCOPY_BOXDIM + 1;
  tft.fillRect(xPos, yPos, LPCOPY_BOXDIM - 2, LPCOPY_BOXDIM - 2, lpColor2tftColor(LPcolor));
}

void LPcopy_setStepIndicator()
{
  static uint8_t previousColumn = 0;
  //uint16_t  ticksPerColumn = ticksPerBeat/LP1.xZoomLevel;
  uint8_t column = tracks[currentTrack]->getCurrentColumn(TICKS_PER_COLUMN);
 
  if (column != previousColumn)
  {
    uint8_t page =  column/8;
    LPcopy_setColumnColor(page, column, LP_RED);
    LPcopy_updateColumn(previousColumn);
    previousColumn = column;
  }
}

void LPcopy_setColumnColor(uint8_t page, uint8_t column, uint8_t LPcolor)
{
   for (uint8_t row = 0; row < 8; row++) { LPcopy_updateSingleEvent(page, (column - page * 8), row, LPcolor); }
}

// LAUNCHPAD UI FUNCTIONS
// ********************************************

void LPinit()
{
  LP1.setPadColor(CCstartStop, LP_RED);
  LP1.setPadColor(CCeditMode, LP_RED);
  setLPpatternMode();
}

void updateLaunchpadUI()
{
  static uint8_t oldLPmode = LPMODE_PATTERN;
  
  if (LPdisplayMode == LPMODE_PATTERN)
  {
    static uint8_t oldTrack = 255;
    static uint8_t oldPattern = 255;
    if ( (currentTrack != oldTrack) || (oldPattern != currentPattern) || ( oldLPmode != LPMODE_PATTERN ))
    {
      oldTrack = currentTrack;
      oldPattern = currentPattern;
      LP1.setActiveTrack(29 + currentTrack * 10 , tracks[currentTrack]->color);
      LPsetPageFromTrackData();
      oldLPmode = LPMODE_PATTERN;
    }
    if (sequencerState == STATE_RUNNING) LPsetStepIndicator();
  }
  
  if ((LPdisplayMode == LPMODE_SCENE))
  {
    LPsetPageFromSceneData(oldLPmode != LPMODE_SCENE);
    LPsetTrackButtonsSceneMode(oldLPmode != LPMODE_SCENE);
    LPsetSceneButtonsSceneMode(oldLPmode != LPMODE_SCENE);
    oldLPmode = LPMODE_SCENE;
  }
 
   if ((LPdisplayMode == LPMODE_SONG))
   {
      LPsetPageFromSongData(oldLPmode != LPMODE_SONG);
      LPsetSceneButtonsSongMode(oldLPmode != LPMODE_SONG);
      oldLPmode = LPMODE_SONG;
   }
}

void LPsetPageFromSongData(bool forceUpdate)
{
  static uint8_t oldArrangement[NR_ARRPOSITIONS];

  for (uint8_t arrId = 0;arrId < NR_ARRPOSITIONS; arrId++)
  {
    if(SequencerData.arrangement[arrId] != oldArrangement[arrId] || forceUpdate)
    {
      oldArrangement[arrId] = SequencerData.arrangement[arrId];
      uint8_t row = 8 - arrId / 8;
      uint8_t column = arrId % 8;
      uint8_t padId = 10 * row + 1 + column;
      //Serial.printf("R: %d, C: %d, Pad: %d\n", row, column, padId);
      if (SequencerData.arrangement[arrId] != NO_SCENE)
      {
        uint8_t sceneId = SequencerData.arrangement[arrId];
        uint8_t color = SequencerData.sceneColors[sceneId];
        LP1.setPadColor(padId, color);
      }
      else LP1.setPadColor(padId, LP_OFF);
    }
  }
}


void LPsetTrackButtonsSceneMode(bool forceUpdate)
{
  static uint8_t muteStatus[7] = {5,5,5,5,5,5,5};
  for (uint8_t trackId = 0; trackId < 7; trackId++)
  {
    uint8_t padId = 29 + trackId * 10;
    uint8_t tempMuteStatus = tracks[trackId]->getMuteStatus();
    if (tempMuteStatus != muteStatus[trackId] || forceUpdate)
    {
      muteStatus[trackId] = tempMuteStatus;
      switch (tracks[trackId]->getMuteStatus())
      {
        case MUTE_OFF:
          LP1.setPadColor(padId, tracks[trackId]->color);
          break;
        case MUTE_ON:
          LP1.setPadColor(padId, LP_RED);
          break;
        case MUTE_ON_CUED:
          LP1.setPadColorFlashing(padId, LP_RED);
          break;
        case MUTE_OFF_CUED:
          LP1.setPadColorFlashing(padId, LP_RED);
          break;
      }
    }
  }
}

void LPsetSceneButtonsSongMode(bool forceUpdate)
{
  if (forceUpdate)
  {
    for (uint8_t column = 0; column < 8; column++)
    {
      uint8_t sceneId = constrain(column + LP1.page *8, 0, NR_SCENES);
      LP1.setPadColor(11 +  column, SequencerData.sceneColors[sceneId]);
    }
  }
}

void LPsetSceneButtonsSceneMode(bool forceUpdate)
{
  static uint8_t oldSelectedSceneId = 255;
  if (oldSelectedSceneId != currentScene || forceUpdate)
  {
    oldSelectedSceneId = currentScene;
    LP1.setRowColor(0, LP_GHOST);
    if ( (currentScene < (8 + LP1.page * 8)) && (currentScene >= (LP1.page * 8)) ) LP1.setPadColor(11 + ( currentScene - LP1.page * 8) , SequencerData.sceneColors[currentScene]);
  }
}

void LPtoggleMute(uint8_t trackId)
{
  bool muteStatus = !tracks[trackId]->trackMuted;
  if (sequencerState == STATE_RUNNING) tracks[trackId]->cueMuteStatus(muteStatus);
  else tracks[trackId]->trackMuted = muteStatus;
  LPsetTrackButtonsSceneMode(false);
}

void LPsetPageFromSceneData(bool forceUpdate)
{
  for (uint8_t trackId = 0; trackId < 7; trackId++) LPsetTrackRowFromSceneData(trackId, forceUpdate);
}

void LPsetTrackRowFromSceneData(uint8_t trackId, bool forceUpdate)
{
  static uint8_t patternStatuses[NR_TRACKS][NR_PATTERNS];
  for (uint8_t arrIndex = 0; arrIndex < NR_PATTERNS; arrIndex++)
  {
    uint8_t color = tracks[trackId]->color;
    uint8_t padId = 21 + trackId * 10 + arrIndex;
    uint8_t currentPatternStatus = tracks[trackId]->getPatternStatus(arrIndex);
    if ( ( currentPatternStatus != patternStatuses[trackId][arrIndex] ) || forceUpdate )
    {
      patternStatuses[trackId][arrIndex] = currentPatternStatus;
      switch (currentPatternStatus)
      {
        case PATTERN_ACTIVE:
          LP1.setPadColor(padId, color);
          break;
        case PATTERN_CUED:
          LP1.setPadColorFlashing(padId, color);
          break;
        default:
          LP1.setPadColor(padId, LP_OFF);
          break;
      }
    }
  }
}

void LPsetStepIndicator()
{
  static uint8_t previousColumn = 0;
  uint8_t column = tracks[currentTrack]->getCurrentColumn(TICKS_PER_COLUMN);
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
  if (LPdisplayMode == LPMODE_PATTERN && ((LP1.page * 8 + 7) < (tracks[currentTrack]->getPatternLengthColumns(TICKS_PER_COLUMN) - 1)))
  {
    LP1.page = LP1.page + 1;
    LPsetPageFromTrackData();
  }
  if ( (LPdisplayMode == LPMODE_SCENE) && (LP1.page < NR_SCENES/8 ) )
  {
    LP1.page = LP1.page + 1;
    LPsetSceneButtonsSceneMode(true);
  }
  if ( (LPdisplayMode == LPMODE_SONG) && (LP1.page < NR_SCENES/8 ) )
  {
    LP1.page = LP1.page + 1;
    LPsetSceneButtonsSongMode(true);
  }
  
}

void LPpageDecrease()
{
  if (LPdisplayMode == LPMODE_PATTERN && LP1.page > 0)
  {
    LP1.page = LP1.page - 1;
    LPsetPageFromTrackData();
  }
  if ( ((LPdisplayMode == LPMODE_SCENE) || (LPdisplayMode == LPMODE_SONG)) && LP1.page > 0)
  {
    LP1.page = LP1.page - 1;
    LPsetSceneButtonsSceneMode(true);
  }
}

void LPscrollUp()
{
  if (LPdisplayMode == LPMODE_PATTERN)
  {
    if (tracks[currentTrack]->lowerRow + LP1.yScrollStep < 127) tracks[currentTrack]->lowerRow = tracks[currentTrack]->lowerRow + LP1.yScrollStep;
    else tracks[currentTrack]->lowerRow = 120;
    LPsetPageFromTrackData();
  }
}

void LPscrollDown()
{
  if (LPdisplayMode == LPMODE_PATTERN)
  {
    if (tracks[currentTrack]->lowerRow >= LP1.yScrollStep) tracks[currentTrack]->lowerRow = tracks[currentTrack]->lowerRow - LP1.yScrollStep;
    else tracks[currentTrack]->lowerRow = 0;
    LPsetPageFromTrackData();
  }
}

void LPsetPageFromTrackData()
{
  for (uint8_t padColumn = 0; padColumn < 8; padColumn++) { LPsetColumnFromTrackData(padColumn); }
}

void LPsetColumnFromTrackData(uint8_t padColumn)
{
  // This function considers which page is currently displayed so only call with column 0 - 7.
  uint8_t padStateArray[24];
  uint16_t tickTemp = (padColumn + LP1.page * 8) * TICKS_PER_COLUMN;
  uint8_t lowerRow = tracks[currentTrack]->lowerRow;
  
  for (uint8_t row = 0; row < 8; row++)
  {
    uint8_t padState = LP_OFF;
    uint8_t padId = 11 + padColumn + row * 10;
    //note: pad row 0 represents note value (lowerRow)
   
    if ((row == 0) && (lowerRow > 0) && (tracks[currentTrack]->getEventsInTickNoteInterval(tickTemp, tickTemp + TICKS_PER_COLUMN - 1, 0, lowerRow - 1) > 0)) padState = LP_GHOST;         // notes below current column
    if ((row == 7) && (lowerRow < 120) && (tracks[currentTrack]->getEventsInTickNoteInterval(tickTemp, tickTemp + TICKS_PER_COLUMN - 1, lowerRow + 7 + 1, 127) > 0)) padState = LP_GHOST; // notes above current column
    if (tracks[currentTrack]->getEventsInTickInterval(tickTemp, tickTemp + TICKS_PER_COLUMN - 1 , lowerRow + row) > 0) padState = tracks[currentTrack]->color;
      
    padStateArray[3 * row] = 0; // solid color
    padStateArray[3 * row + 1] = padId;
    padStateArray[3 * row + 2] = padState;
  }
  LP1.setMultiplePadColorState(padStateArray, 24);
}

void setLPsongMode()
{
  uint8_t padStateArray[9] = {0, CCsongMode, LP_PURPLE, 0, CCsceneMode, LP_OFF, 0, CCpatternMode, LP_OFF};
  LP1.setMultiplePadColorState(padStateArray, 9);
  LPdisplayMode = LPMODE_SONG;
  LP1.page = 0;
}

void setLPsceneMode()
{
  uint8_t padStateArray[9] = {0, CCsongMode, LP_OFF, 0, CCsceneMode, LP_BLUE, 0, CCpatternMode, LP_OFF};
  LP1.setMultiplePadColorState(padStateArray, 9);
  LPdisplayMode = LPMODE_SCENE;
  LP1.page = 0;
}

void setLPpatternMode()
{
  uint8_t padStateArray[9] = {0, CCsongMode, LP_OFF, 0, CCsceneMode, LP_OFF, 0, CCpatternMode, LP_GREEN};
  LP1.setMultiplePadColorState(padStateArray, 9);
  LPdisplayMode = LPMODE_PATTERN;
  LP1.page = 0;
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
    case LP_YELLOW: return TFT_YELLOW;
    case LP_OFF: return TFT_OFF;
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
  if (bouncers[buttonNr].fallingEdge()) return 1;
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
        currentPage = PAGE_SCENE;
        //mcp.writePort(MCP23017Port::A, 4);
        break;
      case 2:
        currentPage = PAGE_PATTERN;
        //mcp.writePort(MCP23017Port::A, 2);
        break;
      case 1:
        currentPage = PAGE_EVENT;
        //mcp.writePort(MCP23017Port::A, 1);
        break;
      case 16:
        currentPage = PAGE_FILE;
        //mcp.writePort(MCP23017Port::A, 16);
        break;
      case 32:
        break;
      case 64:
        currentPage = PAGE_TOOLS;
        break;
      case 128:
        currentPage = PAGE_TRACK;
        //currentPage = PAGE_LISTDEVICES;
        //mcp.writePort(MCP23017Port::A, 128);
        break;
        
    }
  }
}
