#include "voice.h"

const float NOTEFREQS[128] = {8.176f, 8.662f, 9.177f, 9.723f, 10.301f, 10.913f, 11.562f, 12.25f, 12.978f, 13.75f,
                              14.568f, 15.434f, 16.352f, 17.324f, 18.354f, 19.445f, 20.602f, 21.827f, 23.125f, 24.5f,
                              25.957f, 27.5f, 29.135f, 30.868f, 32.703f, 34.648f, 36.708f, 38.891f, 41.203f, 43.654f,
                              46.249f, 48.999f, 51.913f, 55.0f, 58.27f, 61.735f, 65.406f, 69.296f, 73.416f, 77.782f,
                              82.407f, 87.307f, 92.499f, 97.999f, 103.826f, 110.0f, 116.541f, 123.471f, 130.813f, 138.591f,
                              146.832f, 155.563f, 164.814f, 174.614f, 184.997f, 195.998f, 207.652f, 220.0f, 233.082f, 246.942f,
                              261.626f, 277.183f, 293.665f, 311.127f, 329.628f, 349.228f, 369.994f, 391.995f, 415.305f, 440.0f,
                              466.164f, 493.883f, 523.251f, 554.365f, 587.33f, 622.254f, 659.255f, 698.456f, 739.989f, 783.991f,
                              830.609f, 880.0f, 932.328f, 987.767f, 1046.502f, 1108.731f, 1174.659f, 1244.508f, 1318.51f, 1396.913f,
                              1479.978f, 1567.982f, 1661.219f, 1760.0f, 1864.655f, 1975.533f, 2093.005f, 2217.461f, 2349.318f, 2489.016f,
                              2637.02f, 2793.826f, 2959.955f, 3135.963f, 3322.438f, 3520.0f, 3729.31f, 3951.066f, 4186.009f, 4434.922f,
                              4698.636f, 4978.032f, 5274.041f, 5587.652f, 5919.911f, 6271.927f, 6644.875f, 7040.0f, 7458.62f, 7902.133f,
                              8372.018f, 8869.844f, 9397.273f, 9956.063f, 10548.08f, 11175.3f, 11839.82f, 12543.85f};

//                      
// _player    ->   _generatorMixer ->  _filter ->  _envelopeAMP-------------------- -> main out -> i2s + usb
// _osc1      -^                       | 
// _osc2      -^                       |
//                 _filterModMixer -^
// _DCfilter -^ 

voice::voice(uint8_t id)
{
  _outputConnectionIndex = 0;
  _internalConnectionIndex = 0;
  _sample = AudioSampleKick;
  _parameters.sideChainSendChannel = id;
  
  // GENERATORS
  _connect(_player, 0, _generatorMixer, 0);
  _connect(_osc1, 0, _generatorMixer, 1);
  _connect(_osc2, 0, _generatorMixer, 2);
  _connect(_noise, 0, _generatorMixer, 3);
  _osc1.begin(1.0, 40.0, WAVEFORM_BANDLIMIT_SAWTOOTH);
  _osc2.begin(1.0, 40.2, WAVEFORM_BANDLIMIT_SAWTOOTH);
  _generatorMixer.gain(0, _parameters.sample_level); // sample
  _generatorMixer.gain(1, _parameters.osc1_level); // osc 1
  _generatorMixer.gain(2, _parameters.osc2_level); // osc 2
  _generatorMixer.gain(3, _parameters.noise_level); // osc 2
  
  // FILTER
  _connect(_generatorMixer, 0, _filter, 0);
  _connect(_dcFilter, 0, _filterModMixer, 0);
  _connect(_dcFilter, 0, _filterEnvelope, 0);
  _connect(_filterEnvelope, 0, _filterModMixer, 1);
  _connect(_filterModMixer, 0, _filter, 1);
  _filter.octaveControl(3);
  _dcFilter.amplitude(1.0);
  _filterModMixer.gain(0, 1.0 - _parameters.filterEnvelopeAmplitude);
  _filterModMixer.gain(1, _parameters.filterEnvelopeAmplitude);
  _filterEnvelope.attack(_parameters.filterEnvelope_attack);
  _filterEnvelope.decay(_parameters.filterEnvelope_decay);
  _filterEnvelope.sustain(_parameters.filterEnvelope_sustain);
  _filterEnvelope.release(_parameters.filterEnvelope_release);

  // AMP ENVELOPE
  _connect(_filter, 0, _ampEnvelope, 0);
  _ampEnvelope.attack(_parameters.ampEnvelope_attack);
  _ampEnvelope.decay(_parameters.ampEnvelope_decay);
  _ampEnvelope.sustain(_parameters.ampEnvelope_sustain);
  _ampEnvelope.release(_parameters.ampEnvelope_release);

  // WAVE FOLDER
   _connect(_ampEnvelope, 0, _waveFolder, 0);
   _connect(_dcWaveFolder, 0, _waveFolder, 1);
   _dcWaveFolder.amplitude(_parameters.waveFolderGain);
    
  // SIDECHAIN
  _connect(_waveFolder, 0, _scMultiplier, 1);
  _connect(_dcSC, 0, _scEnvelope, 0);
  _connect(_scEnvelope, 0, _scMixer, 0);
  _connect(_dcSC, 0, _scMixer, 1);
  _connect(_scMixer, 0, _scMultiplier, 0);
  _connect(_scMultiplier, 0, _voiceOutput, 0);
  _scEnvelope.attack(_parameters.sideChainEnvelope_attack);
  _scEnvelope.hold(_parameters.sideChainEnvelope_hold);
  _scEnvelope.decay(_parameters.sideChainEnvelope_decay);
  _scEnvelope.sustain(_parameters.sideChainEnvelope_sustain);
  _scEnvelope.release(_parameters.sideChainEnvelope_release);
  _dcSC.amplitude(1.0);
  _scMixer.gain(1, 1.0);

  if (id == 0)
  {
    _generatorMixer.gain(0, 0); // sample
    _generatorMixer.gain(1, 0.8); // osc 1
    _generatorMixer.gain(2, 0.8); // osc 2
    _generatorMixer.gain(3, 0.1); // noise
    _parameters.sideChainReceiveChannel = 1;
    _parameters.sideChainLevel = 0.0;
    _filter.frequency(150);
    _filter.resonance(2.0);
    _filterEnvelope.attack(0);
    _filterEnvelope.decay(200);
    _filterEnvelope.sustain(0.2);
    _filterEnvelope.release(300);
    _ampEnvelope.attack(0);
    _ampEnvelope.decay(200);
    _ampEnvelope.sustain(0.4);
    _ampEnvelope.release(200.0);
  }
 
  _scMixer.gain(0, -_parameters.sideChainLevel);
}

void voice::noteOn(uint8_t note, uint8_t velocity)
{
  if (_sample != nullptr) _player.play(_sample);
  setOscFrequency(note);
  _ampEnvelope.noteOn();
  _filterEnvelope.noteOn();
}

void voice::noteOff(uint8_t note, uint8_t velocity)
{
  _ampEnvelope.noteOff();
  _filterEnvelope.noteOff();
}

void voice::setOscFrequency(uint8_t note)
{
  uint8_t osc1_baseFreq = NOTEFREQS[constrain(note + _parameters.osc1_transpose, 0, 127)];
  uint8_t osc2_baseFreq = NOTEFREQS[constrain(note + _parameters.osc2_transpose, 0, 127)];
  _osc1.frequency(osc1_baseFreq * pow(2, -_parameters.detune));
  _osc2.frequency(osc2_baseFreq * pow(2, _parameters.detune));
}

void voice::triggerSideChain(uint8_t sourceChannel)
{
  if ( (sourceChannel == _parameters.sideChainReceiveChannel ) && ( _parameters.sideChainLevel > 0.0 ) ) _scEnvelope.noteOn();  
}

void voice::_connect(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput)
{
  _patchCords[_internalConnectionIndex] = new AudioConnection(source, sourceOutput, destination , destinationInput);
  _internalConnectionIndex++;
  if (_internalConnectionIndex > NR_PATCHCORDS_VOICE - 1)
  {
    _internalConnectionIndex = NR_PATCHCORDS_VOICE - 1;
    Serial.println("ERROR: Voice backend - no available patchcords");
  }
}

void voice::connectToMixer(AudioMixer4 & targetMixer, uint8_t targetMixerPortIndex)
{
  delete _outputConnection[_outputConnectionIndex];
  _outputConnection[_outputConnectionIndex] = new AudioConnection(_voiceOutput, 0, targetMixer, targetMixerPortIndex);
}

String getOscEnum(uint8_t value)
{
   if (value == 0) return "SINE"; // 0
   if (value == 1) return "SAW"; // 1
   if (value == 2) return "PULSE"; // 2
   if (value == 3) return "ARBITR"; // 3
   else return "ERR";
}
