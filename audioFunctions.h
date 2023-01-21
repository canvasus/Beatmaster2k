#pragma once

#include <Audio.h>
#include "src/SM2k_control_wm8731.h"
#include "src/effect_dynamics.h"
#include "src/effect_platervbstereo.h"

// --- Bd ---
#include "src/samples/AudioSampleBd_linn.h"

// --- Sd ---
#include "src/samples/AudioSampleSd_linn.h"

// --- Hh ---
#include "src/samples/AudioSampleHh_rytmmpc60_2.h"

// --- Ho ---
#include "src/samples/AudioSampleHho_rytmmpc60_3.h"

// --- Cy ---

// --- Tm ---
#include "src/samples/AudioSampleToml_linn.h"
#include "src/samples/AudioSampleTom_linn.h"
#include "src/samples/AudioSampleTomh_linn.h"
#include "src/samples/AudioSampleTomhh_linn.h"

#define NR_PLAYERS 8
#define NR_SAMPLES 8

struct drumKitParameters
{
  uint8_t sampleId[NR_PLAYERS] = {0, 1, 2, 3, 4, 5, 6, 7};
  
  float level[NR_PLAYERS] = {0.8, 0.8, 0.2, 0.2, 0.8, 0.8, 0.8, 0.8,}; 
  int pan[NR_PLAYERS] = {0, 0, 0, 0, -60, -25, 25, 60};
  int choke[NR_PLAYERS] = {-1, -1, 3, 2, -1, -1, -1, -1};
   
  float dynThreshold = -2.0;
  float dynAttack = 0.03;
  float dynRelease = 0.25;
  float dynRatio = 2;
  float dynKneeWidth = 0.0;
  float dynInGain = 0.0;
  
  float reverb_size = 0.75;
  float reverb_hidamp = 0.7;
  float reverb_lodamp = 0.7;
  float reverb_lowpass = 0.4;
  float reverb_diffusion = 0.65;

  float mainOut_dry = 1.0;
  float mainOut_reverb = 0.4;
      
};

void setupAudio();

void voiceNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void voiceNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);

void setPlayerLevel(uint8_t playerId, float level);
void setPlayerPanning(uint8_t playerId, float pan);
void setPlayerMix(uint8_t playerId);
