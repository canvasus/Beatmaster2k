#pragma once

#include <ILI9341_t3.h>
#include <font_Arial.h> // from ILI9341_t3
#define SCREEN_YRES 240
#define SCREEN_XRES 320

#define TFT_DC  9
#define TFT_CS 10
// MOSI=11, MISO=12, SCK=13

#include <Encoder.h>
#include <Bounce.h>
#include "audioFcns.h"

#define ROT1_A 41   // GPIO_A17, J2 pin 7
#define ROT1_B 40   // GPIO_A16, J2 pin 8
#define ROT1_SW 2   // GPIO_D2, J2 pin 10
                    // GND, J2 pin 9

#define ROT2_A 3    // GPIO_D3, J2 pin 11
#define ROT2_B 4    // GPIO_D4, J2 pin 13
#define ROT2_SW 15  // GPIO_A1, J2 pin 14
                    // GND, J2 pin 12

#define Y_SCALER 1000

#define PAGE_HOME       0
#define PAGE_OSC        1
#define PAGE_AMPENV     2
#define PAGE_FILTERENV  3
#define PAGE_MOD        4
#define PAGE_WAVEFORM   5
#define PAGE_CHORUS     6
#define PAGE_REVERB     7
#define PAGE_MIX        8

#define AMP_ENV   0
#define FILTER_ENV  1

#define BUTTON_HEIGHT 14
#define BUTTON_WIDTH  62

#define OSC_VAR0  0
#define OSC_VAR1  1
#define OSC_VAR2  2
#define OSC_VAR3  3

#define CHORUS_LFORATE 12
#define REVERB_SIZE 13
#define REVERB_LODAMP 14
#define REVERB_HIDAMP 15
#define REVERB_DIFFUSION 16
#define MIX_DRY 17
#define MIX_CHORUS 18
#define MIX_REVERB 19

#define NR_PARAMETERS 20

#define DISPLAYMODE_SEQUENCER      0
#define DISPLAYMODE_MIXER          1
#define DISPLAYMODE_NOTELENGTH     2
#define DISPLAYMODE_PATTERNSELECT  3
#define DISPLAYMODE_PATTERNLENGTH  4

extern uint8_t displayMode;

typedef float (* FPgetFloat)();
typedef void (* FPsetFloat)(float);

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
};

extern audioBackend AudioBackend;

void setupUI();

void updateUI();
void updateDisplayUI();
void updateLaunchpadUI();

void updateNavigationPage(uint8_t * pageArray, uint8_t nrButtons, bool firstCall);
void drawMenuButton(uint8_t index, String text, bool selected);

void updateParameterPage(const uint8_t * parameterArray, const uint8_t nrParameters, bool firstCall);
void drawParameterRow(uint8_t index, String name, float value, uint8_t precision, bool selected);
void updateParameterRow(uint8_t index, float value, uint8_t precision);
void drawParameterRowEnum(uint8_t index, String name, String value, bool selected);
void updateParameterRowEnum(uint8_t index, String value);

float getValue(uint8_t parameter);
void updateValue(uint8_t parameter, float value);

void updatePageHome(bool firstCall);
void updatePageMix(bool firstCall);

int16_t getEncoderDirection(uint8_t encoderNr);
uint8_t updateButton(uint8_t buttonNr);
