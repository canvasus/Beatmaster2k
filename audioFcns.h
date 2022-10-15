#pragma once

#include <Audio.h>
#include "voice.h"
#include "src/effect_ensemble.h"
#include "src/effect_platervbstereo.h"
#include "src/SM2k_control_wm8731.h"

#define NR_PATCHCORDS 32

extern voice Voices[NR_VOICES];

struct audioParameters
{
  float hpVolume = 0.70;
  float voiceMixerGain = 0.5;
  float usbGain = 2.0;
  float chorusSendGain = 1.0;

  float mainOut_dry = 1.0;
  float mainOut_chorus = 0.5;
  float mainOut_reverb = 0.3;
  
  float chorus_lfoRate = 2.0;

  float reverb_size = 0.50;
  float reverb_hidamp = 0.5;
  float reverb_lodamp = 0.1;
  float reverb_lowpass = 0.4;
  float reverb_diffusion = 0.65;
  
};

class audioBackend
{
  private:
    AudioOutputI2SQuad   _audioOutput1;
    AudioOutputUSB       _audioOutputUSB;
    AudioConnection  *   _patchCords[NR_PATCHCORDS];
    AudioMixer4          _voiceMixers[3]; // for 8 voices
    AudioEffectEnsemble  _ensemble;
    AudioEffectPlateReverb _reverb;

    AudioAmplifier       _chorusSend;
    AudioAmplifier       _reverbSendL;
    AudioAmplifier       _reverbSendR;
    
    AudioMixer4          _mainOutL;
    AudioMixer4          _mainOutR;

    AudioAmplifier       _usbGainL;
    AudioAmplifier       _usbGainR;
      
    //AudioControlSGTL5000 _sgtl5000;
    SM2k_AudioControlWM8731 codecControl1;
    SM2k_AudioControlWM8731 codecControl2;
    audioParameters      _parameters;
    uint16_t              _internalConnectionIndex;
    void _connect(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput);
  public:
    audioBackend();
    void setupAudio();
    float getDryLevel();
    float getChorusLevel();
    float getReverbLevel();
    void setDryLevel(float level);
    void setChorusLevel(float level);
    void setReverbLevel(float level);
    
};

extern audioBackend AudioBackend;

// HELPER FUNCTIONS FOR UI
// **************************
void setDryLevel(float level);
void setChorusLevel(float level);
void setReverbLevel(float level);
float getDryLevel();
float getChorusLevel();
float getReverbLevel();

void displayResources();
