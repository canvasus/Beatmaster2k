#pragma once
//
//#include <Audio.h>
//#include "src/samples/AudioSampleKick.h"
//
//#define NR_SAMPLES 7
//
//#define NR_VOICES 1
//#define NR_PATCHCORDS_VOICE 24
//
//struct voiceParameters
//{
//  float sample_level = 1.0;
//  float osc1_level = 0.0;
//  float osc2_level = 0.0;
//  float noise_level = 0.0;
//  uint8_t osc1Waveform = WAVEFORM_BANDLIMIT_SAWTOOTH;
//  uint8_t osc2Waveform = WAVEFORM_BANDLIMIT_SAWTOOTH;
//  float detune = 0.002;
//  int8_t osc1_transpose = 0;
//  int8_t osc2_transpose = 0;
//  
//  float ampEnvelope_attack = 0;
//  float ampEnvelope_decay = 0;
//  float ampEnvelope_sustain = 1.0;
//  float ampEnvelope_release = 10.0;
//
//  float filterCutoff = 2000.0;
//  float filterResonance = 0.7;
//  float filterEnvelope_attack = 0;
//  float filterEnvelope_decay = 0;
//  float filterEnvelope_sustain = 1.0;
//  float filterEnvelope_release = 10.0;
//  float filterEnvelopeAmplitude = 1.0;
//
//  float waveFolderGain = 0.10;
//
//  float sideChainEnvelope_attack = 0;
//  float sideChainEnvelope_hold = 30;
//  float sideChainEnvelope_decay = 100;
//  float sideChainEnvelope_sustain = 0.0;
//  float sideChainEnvelope_release = 0.0;
//
//  uint8_t sideChainSendChannel = 0;
//  uint8_t sideChainReceiveChannel = 255;
//  float sideChainLevel = 0.0;
//
//  
//};
//
//class voice
//{
//  private:
//    const unsigned int *      _sample = nullptr;
//    AudioPlayMemory           _player;
//    AudioSynthWaveform        _osc1;
//    AudioSynthWaveform        _osc2;
//    AudioSynthNoiseWhite      _noise;
//    AudioMixer4               _generatorMixer;
//
//    AudioSynthWaveformDc      _dcFilter;
//    AudioEffectEnvelope       _filterEnvelope;
//    AudioMixer4               _filterModMixer;
//    AudioFilterStateVariable  _filter;
//    
//    AudioEffectEnvelope       _ampEnvelope;
//    
//    AudioSynthWaveformDc      _dcWaveFolder;
//    AudioEffectWaveFolder     _waveFolder;
//
//    AudioSynthWaveformDc      _dcSC;
//    AudioEffectEnvelope       _scEnvelope;
//    AudioMixer4               _scMixer;
//    AudioEffectMultiply       _scMultiplier;
//     
//    AudioConnection *     _patchCords[NR_PATCHCORDS_VOICE];
//    AudioConnection *     _outputConnection[4];
//    uint8_t               _outputConnectionIndex;
//    uint16_t              _internalConnectionIndex;
//    AudioAmplifier        _voiceOutput;
//    voiceParameters       _parameters;
//  
//    void _connect(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput);
//
//  public:
//    voice(uint8_t id);
//    void connectToMixer(AudioMixer4 & targetMixer, uint8_t targetMixerPortIndex);
//    void noteOn(uint8_t note, uint8_t velocity);
//    void noteOff(uint8_t note, uint8_t velocity);
//    void triggerSideChain(uint8_t sourceChannel);
//    void setOscFrequency(uint8_t note);
//};
//
//String getOscEnum(uint8_t value);
