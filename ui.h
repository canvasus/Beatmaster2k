#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
//#include <Encoder.h>
//#include <Bounce.h>
#include "audioFcns.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

#define ROT1_A 5
#define ROT1_B 2
#define ROT1_SW 4

#define ROT2_A 9
#define ROT2_B 3
#define ROT2_SW 22

#define SW_A 1
#define SW_B 15
#define SW_C 14

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
