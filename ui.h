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
#include "audioFcns.h"

#define SCREEN_YRES 240
#define SCREEN_XRES 320
#define BUTTON_HEIGHT 30
#define BUTTON_WIDTH  150
#define BUTTON_PADDING 5
#define VAR_NAME_WIDTH 100
#define VAR_VALUE_WIDTH 155

#define HEADER_X        270
#define HEADER_Y        0
#define HEADER_WIDTH    50
#define HEADER_HEIGHT   SCREEN_YRES
#define HEADER_TEXT_X   274
#define HEADER_OFFSET_Y 25  
#define PAGEINDICATOR_HEIGHT 30 
#define PAGEINDICATOR_WIDTH 140 

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

#define PATTERN_LENGTH          0
#define PATTERN_SPEED           1
#define PATTERN_EVENTLENGTHDEF  2 
#define TRACK_OUTPUT            3
#define SONG_BPM                4
#define TRACK_CHANNEL           5
#define EVENT_LENGTH            6
#define TRACK_TRANSPOSESTATUS   7

#define NR_PARAMETERS 25
#define MAX_CHILDREN 8
#define PAGE_NAV 0
#define PAGE_PAR 1

#define W_NONE      0
#define W_LIST      1
#define W_POT       2
#define W_SLIDER    3

#define DISPLAYMODE_SEQUENCER      0
#define DISPLAYMODE_PATTERNSELECT  1

extern uint8_t LPdisplayMode;
extern uint8_t currentTrack;
extern uint8_t currentPattern;
extern int16_t currentEvent;

typedef float (* FPgetFloat)();
typedef void (* FPsetFloat)(float);
typedef String (* FPgetString)(uint8_t);

struct page
{
  uint8_t pageType;
  char name[8];
  uint8_t pageBack;
  uint8_t nrChildren;
  uint8_t children[MAX_CHILDREN];
  uint8_t widget;
};

struct parameters
{
  char name[8];
  float value;
  float minValue;
  float maxValue;
  float multiplier;
  uint8_t displayPrecision;
  FPgetFloat getFunction = nullptr;
  FPsetFloat setFunction = nullptr;
  FPgetString enumFunction = nullptr;
};

extern audioBackend AudioBackend;
extern Track *tracks[NR_TRACKS];

void setupUI();
void showStartupScreen();


void updateUI();
void updateDisplayUI();
void updateLaunchpadUI();

bool updateHeader(uint8_t trackNr, uint8_t patternNr, uint8_t bpm, bool firstCall);

void updateNavigationPage(const uint8_t * pageArray, uint8_t nrButtons, bool firstCall);
void clearNavigationArea();
void drawMenuButton(uint8_t index, String text, bool selected);

void updateParameterPage(const uint8_t * parameterArray, const uint8_t nrParameters, bool firstCall, bool forceVariableUpdate);
void drawParameterRow(uint8_t index, uint8_t parameter, bool selected);
void updateParameterRow(uint8_t index, uint8_t parameter);

void drawPotWidget(uint8_t index, uint8_t parameter, bool selected, bool drawStatics);
void updatePotWidget(uint8_t index, uint8_t parameter);

float getValue(uint8_t parameter);
void updateValue(uint8_t parameter, float value);

void handleSpecialPages(bool firstCall);
void displayDevicePage(bool firstCall);

void LPinit();
void LPsetStepIndicator();
void LPpageIncrease();
void LPpageDecrease();
void LPscrollUp();
void LPscrollDown();
void LPsetPageFromTrackData();
void LPsetColumnFromTrackData(uint8_t padColumn);
void LPdisplayPatternPage(bool firstCall);
void setLPsongMode();
void setLPpatternMode();

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
