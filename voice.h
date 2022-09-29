#pragma once

#include <Audio.h>
#include "src/samples/x_kick1.h"
#include "src/samples/AudioSampleBd88.h"
#include "src/samples/AudioSampleSd88.h"
#include "src/samples/AudioSampleSidsd.h"
#include "src/samples/AudioSample8o8ch1.h"
#include "src/samples/AudioSampleLastninja2.h"

#define NR_SAMPLES 4

#define NR_VOICES 8
#define NR_PATCHCORDS_VOICE 24

struct voiceParameters
{
  float cutoff = 2000.0;
  float resonance = 0.7;
  
  float ampEnvelope_attack = 30;
  float ampEnvelope_decay = 100;
  float ampEnvelope_sustain = 0.9;
  float ampEnvelope_release = 500.0;

  float filterEnvelope_attack = 30;
  float filterEnvelope_decay = 100;
  float filterEnvelope_sustain = 0.9;
  float filterEnvelope_release = 500.0;

  float sideChainEnvelope_attack = 10;
  float sideChainEnvelope_hold = 50;
  float sideChainEnvelope_decay = 100;
  float sideChainEnvelope_sustain = 0.0;
  float sideChainEnvelope_release = 0.0;

  uint8_t sideChainSendChannel = 0;
  uint8_t sideChainReceiveChannel = 255;
  float sideChainLevel = 0.0;
};

class voice
{
  private:
    const unsigned int *      _sample = nullptr;
    AudioPlayMemory           _player;
    AudioEffectEnvelope       _envelope1;
    AudioSynthWaveformDc      _dcFilter;
    AudioSynthWaveformDc      _dcWaveFolder;
    AudioFilterStateVariable  _filter1;
    AudioEffectWaveFolder     _wavefolder1;

    // SideChain
    AudioSynthWaveformDc      _dcSC;
    AudioEffectEnvelope       _envelopeSC;
    AudioMixer4               _SCmixer;
    AudioAnalyzePeak          _SCout;
    AudioEffectMultiply     _SCmultiplier;

    AudioSynthWaveform      _testTone;
    
    AudioConnection *     _patchCords[NR_PATCHCORDS_VOICE];
    AudioConnection *     _outputConnection[4];
    uint8_t               _outputConnectionIndex;
    uint16_t              _internalConnectionIndex;
    AudioAmplifier        _output;
    voiceParameters       _parameters;
  
    void _connect(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput);

  public:
    voice(uint8_t id);
    void connectToMixer(AudioMixer4 & targetMixer, uint8_t targetMixerPortIndex);
    void noteOn(uint8_t note, uint8_t velocity);
    void noteOff(uint8_t note, uint8_t velocity);
    void triggerSideChain(uint8_t sourceChannel);
    float getSCdata();
};
