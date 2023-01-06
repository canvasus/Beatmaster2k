#pragma once

#include "track.h"
#include "midiFunctions.h"
#include "sequencer.h"
#include <ILI9341_t3.h>
#include <font_Arial.h> // from ILI9341_t3
#include <Wire.h>
#include <MCP23017.h>
#include <Encoder.h>
#include <Bounce.h>
#include "sdMgr.h"

#define SCREEN_YRES           240
#define SCREEN_XRES           320
#define BUTTON_HEIGHT         26
#define BUTTON_WIDTH          150
#define BUTTON_PADDING        5
#define VAR_NAME_WIDTH        100
#define VAR_VALUE_WIDTH       155
#define MAIN_AREA_HEIGHT      170
#define MAIN_AREA_WIDTH       SCREEN_XRES

#define PAGEINDICATOR_HEIGHT  30 
#define PAGEINDICATOR_WIDTH   100 
#define HEADER_X              PAGEINDICATOR_WIDTH + 1
#define HEADER_Y              0
#define HEADER_WIDTH          SCREEN_XRES - PAGEINDICATOR_WIDTH
#define HEADER_HEIGHT         PAGEINDICATOR_HEIGHT
#define HEADER_TEXT_X         HEADER_X + 5
#define HEADER_OFFSET_X       35

#define MAIN_BG_COLOR         ILI9341_NAVY
#define PAGEINDICATOR_COLOR   ILI9341_BLUE

#define LPCOPY_XPOS           5
#define LPCOPY_YPOS           200
#define LPCOPY_WIDTH          SCREEN_XRES
#define LPCOPY_HEIGHT         40
#define LPCOPY_BOXDIM         4
#define LPCOPY_BOX_OUTLINE_COLOR ILI9341_BLACK
#define LPCOPY_BOX_SELECTED_COLOR ILI9341_LIGHTGREY

#define TFT_DC  9
#define TFT_CS 10
// MOSI=11, MISO=12, SCK=13

#define MCP23017_ADDR 0x20

#define ROT2_B 41   // GPIO_A17, J2 pin 7
#define ROT2_A 40   // GPIO_A16, J2 pin 8
#define ROT2_SW 2   // GPIO_D2, J2 pin 10
                    // GND, J2 pin 9

#define ROT1_B 3    // GPIO_D3, J2 pin 11
#define ROT1_A 4    // GPIO_D4, J2 pin 13
#define ROT1_SW 15  // GPIO_A1, J2 pin 14
                    // GND, J2 pin 12

#define PAGE_SONG         0
#define PAGE_PATTERN      1
#define PAGE_EVENT        2
#define PAGE_TRACK        3
#define PAGE_FILE         4
#define PAGE_LISTDEVICES  5
#define PAGE_SCENE        6
#define PAGE_TOOLS        7
#define PAGE_SAVE         8
#define PAGE_SONG2        9

#define PATTERN_LENGTH          0
#define PATTERN_SPEED           1
#define PATTERN_EVENTLENGTHDEF  2 
#define TRACK_OUTPUT            3
#define SONG_BPM                4
#define TRACK_CHANNEL           5
#define EVENT_LENGTH            6
#define TRACK_TRANSPOSESTATUS   7
#define PATTERN_NR              8
#define SCENE_NR                9
#define SCENE_COLOR             10
#define TOOLS_SELECTION         11
#define TOOLS_ACTION            12
#define TOOLS_TRANSPOSE         13
#define MIDI_CLOCK              14
#define SONG_PLAYMODE           15

#define NR_PARAMETERS 11
#define MAX_CHILDREN 8
#define PAGE_NAV 0
#define PAGE_PAR 1

#define W_NONE      0
#define W_LIST      1
#define W_POT       2
#define W_SLIDER    3

#define DISPLAYMODE_SEQUENCER      0
#define DISPLAYMODE_PATTERNSELECT  1

#define LPMODE_SCENE    0
#define LPMODE_PATTERN  1
#define LPMODE_SONG     2
#define LPMODE_KEYBOARD 3

#define KEY_VAR_UI_PAGE    0
#define KEY_VAR_LP_PAGE    1
#define KEY_VAR_TRACK_NR   2
#define KEY_VAR_PATTERN_NR 3
#define KEY_VAR_SCENE_NR   4
#define KEY_VAR_ARRPOS_NR  5

#define NR_KEY_VARS 6

extern uint8_t LPdisplayMode;
extern uint8_t currentFileNr;

typedef float (* FPgetFloat)();
typedef void (* FPsetFloat)(float);
typedef String (* FPgetString)(uint8_t);

struct page
{
  uint8_t pageType;
  char name[9];
  uint8_t nrChildren;
  uint8_t children[MAX_CHILDREN];
};

struct parameters
{
  char name[9];
  float minValue;
  float maxValue;
  float multiplier;
  uint8_t displayPrecision;
  FPgetFloat getFunction = nullptr;
  FPsetFloat setFunction = nullptr;
  FPgetString enumFunction = nullptr;
};

void setupUI();
void showStartupScreen();

void updateUI();
void updateDisplayUI();
void updateLaunchpadUI();

void signalKeyVariableChange();
void updateHeader(bool firstCall);

void clearMainArea();
void drawMenuButton(uint8_t index, String text, bool selected);

void updateParameterPage(const uint8_t * parameterArray, const uint8_t nrParameters, bool firstCall, bool forceVariableUpdate);
void drawParameterRow(uint8_t index, uint8_t parameter, bool selected);
void updateParameterRow(uint8_t index, uint8_t parameter);

float getValue(uint8_t parameter);
void updateValue(uint8_t parameter, float value);

void handleSpecialPages(bool firstCall);
void displayDevicePage(bool firstCall);
void displayFilePage(bool firstCall);
void displayFileNr(uint8_t fileNr);
void displayFileName(uint8_t fileNr);
void displayFileName_strEdit(char * name,  uint8_t markedIndex);
void displayLoadSave();
void displaySavePage(bool firstCall);

void displaySongPage2(bool firstCall);

void displayEncoderButtonHint(uint8_t encoder, String hint, uint16_t color);
void displayToolsPage(bool firstCall);

void LPcopy_update(bool firstCall);
void LPcopy_clearArea();
void LPcopy_drawBackground(uint8_t nrPages);
void LPcopy_setSelectedPage();
void LPcopy_updateSingleEvent_converter(uint8_t note, uint8_t padState);
void LPcopy_updateSingleEvent(uint8_t page, uint8_t column, uint8_t row, uint8_t LPcolor);
void LPcopy_updateAllEvents();
void LPcopy_updateColumn(uint8_t column);
void LPcopy_setStepIndicator();
void LPcopy_setColumnColor(uint8_t page, uint8_t column, uint8_t LPcolor);

void LPinit();
void LPsetStepIndicator();
void LPpageIncrease();
void LPpageDecrease();
void LPscrollUp();
void LPscrollDown();
void LPsetPageFromTrackData();
void LPsetColumnFromTrackData(uint8_t padColumn);
void setLPsongMode();
void setLPsceneMode();
void setLPpatternMode();
void LPsetTrackButtonsSceneMode(bool forceUpdate);
void LPsetSceneButtonsSceneMode(bool forceUpdate);
void LPsetSceneButtonsSongMode(bool forceUpdate);
void LPsetArrEventStatus(uint8_t arrId, bool current);
void LPtoggleMute(uint8_t control);
void LPsetPageFromSceneData(bool forceUpdate);
void LPsetTrackRowFromSceneData(uint8_t trackId, bool forceUpdate);
void LPsetPageFromSongData(bool forceUpdate);

uint16_t lpColor2tftColor(uint8_t lpColor);

int16_t getEncoderDirection(uint8_t encoderNr);
uint8_t updateButton(uint8_t buttonNr);
void initMcp();
void updateMcp();

#define TFT_RED    ILI9341_RED
#define TFT_BLUE   ILI9341_BLUE
#define TFT_GREEN  ILI9341_GREEN
#define TFT_PURPLE ILI9341_PURPLE
#define TFT_CYAN   ILI9341_CYAN
#define TFT_ORANGE ILI9341_ORANGE
#define TFT_PINK   ILI9341_PINK
#define TFT_GHOST  0x1111
#define TFT_DARKBLUE ILI9341_NAVY
#define TFT_YELLOW ILI9341_YELLOW
#define TFT_OFF 0x31A6

//#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
//#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
//#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
//#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
//#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
//#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
//#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
//#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
//#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
//#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
//#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
//#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
//#define ILI9341_RED         0xF800      /* 255,   0,   0 */
//#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
//#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
//#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
//#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
//#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
//#define ILI9341_PINK        0xF81F
