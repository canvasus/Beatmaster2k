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
#define HEADER_HEIGHT 30
#define BUTTON_HEIGHT 30
#define BUTTON_WIDTH  150
#define BUTTON_PADDING 5

#define TFT_DC  9
#define TFT_CS 10
// MOSI=11, MISO=12, SCK=13

#define MCP23017_ADDR 0x20

#define ROT1_B 41   // GPIO_A17, J2 pin 7
#define ROT1_A 40   // GPIO_A16, J2 pin 8
#define ROT1_SW 2   // GPIO_D2, J2 pin 10
                    // GND, J2 pin 9

#define ROT2_B 3    // GPIO_D3, J2 pin 11
#define ROT2_A 4    // GPIO_D4, J2 pin 13
#define ROT2_SW 15  // GPIO_A1, J2 pin 14
                    // GND, J2 pin 12

#define PAGE_HOME       0
#define PAGE_VOICE      1
#define PAGE_AMPENV     2
#define PAGE_FILTERENV  3
#define PAGE_MOD        4
#define PAGE_EFX        5
#define PAGE_MIX        6
#define PAGE_OSC_WFM    7
#define PAGE_OSC_PITCH  8
#define PAGE_CHORUS     9
#define PAGE_REVERB     10
#define PAGE_TRACK      11
#define PAGE_SEQ        12
#define PAGE_LISTDEVICES 13
 
#define OSC1WFM  0
#define OSC2WFM  1
#define OSC2TRANSPOSE  2
#define OSC_DETUNE  3
#define AMPENV_ATTACK 4
#define AMPENV_DECAY  5
#define AMPENV_SUSTAIN 6
#define AMPENV_RELEASE 7
#define FILTERENV_ATTACK 8
#define FILTERENV_DECAY  9
#define FILTERENV_SUSTAIN 10
#define FILTERENV_RELEASE 11
#define CHORUS_LFORATE 12
#define REVERB_SIZE 13
#define REVERB_LODAMP 14
#define REVERB_HIDAMP 15
#define REVERB_DIFFUSION 16
#define MIX_DRY 17
#define MIX_CHORUS 18
#define MIX_REVERB 19
#define TRACK_LENGTH 20
#define TRACK_SPEED 21
#define TRACK_OUTPUT 22
#define SEQ_BPM 23
#define TRACK_CHANNEL 24

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
extern Track *tracks[5];

void setupUI();
void showStartupScreen();


void updateUI();
void updateDisplayUI();
void updateLaunchpadUI();

void updateHeader(uint8_t trackNr, uint8_t patternNr, uint8_t bpm, bool firstCall);

void updateNavigationPage(const uint8_t * pageArray, uint8_t nrButtons, bool firstCall);
void clearNavigationArea();
void drawMenuButton(uint8_t index, String text, bool selected);

void updateParameterPage(const uint8_t * parameterArray, const uint8_t nrParameters, bool firstCall);
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

int16_t getEncoderDirection(uint8_t encoderNr);
uint8_t updateButton(uint8_t buttonNr);
void initMcp();
void updateMcp();

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
