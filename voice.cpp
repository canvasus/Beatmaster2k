#include "voice.h"

voice::voice(uint8_t id)
{
  _outputConnectionIndex = 0;
  _internalConnectionIndex = 0;
  _sample = x_kick1;
  _parameters.sideChainSendChannel = id;

 
  _connect(_dcSC, 0, _envelopeSC, 0);
  _connect(_envelopeSC, 0, _SCmixer, 0);
  _connect(_dcSC, 0, _SCmixer, 1);
  _connect(_SCmixer, 0, _SCmultiplier, 0);
  _connect(_SCmixer, 0, _SCout, 0);
  _connect(_SCmultiplier, 0, _output, 0);

  // Voices[0] is a test tone with sidechain listen to channel 1
  if (id == 0)
  {
    _connect(_testTone, 0, _SCmultiplier, 1);
    _testTone.begin(1.0, 60.0, WAVEFORM_BANDLIMIT_SAWTOOTH);
    _parameters.sideChainReceiveChannel = 1;
    _parameters.sideChainLevel = 0.75;
  }
  else _connect(_player, 0, _SCmultiplier, 1);
  
  _envelopeSC.attack(_parameters.sideChainEnvelope_attack);
  _envelopeSC.hold(_parameters.sideChainEnvelope_hold);
  _envelopeSC.decay(_parameters.sideChainEnvelope_decay);
  _envelopeSC.sustain(_parameters.sideChainEnvelope_sustain);
  _envelopeSC.release(_parameters.sideChainEnvelope_release);

  _dcSC.amplitude(1.0);
  _SCmixer.gain(0, -_parameters.sideChainLevel);
  _SCmixer.gain(1, 1.0);
}

void voice::noteOn(uint8_t note, uint8_t velocity)
{
  _player.play(_sample);
}

void voice::noteOff(uint8_t note, uint8_t velocity)
{
  //_player.stop();
}

void voice::triggerSideChain(uint8_t sourceChannel)
{
  if ( (sourceChannel == _parameters.sideChainReceiveChannel ) && ( _parameters.sideChainLevel > 0.0 ) ) _envelopeSC.noteOn();  
}

float voice::getSCdata()
{
  static float SCdata = 1.0;
  if (_SCout.available()) SCdata = _SCout.read();
  return SCdata;
}

void voice::_connect(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput)
{
  //static uint16_t connectionIndex = 0;
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
  _outputConnection[_outputConnectionIndex] = new AudioConnection(_output, 0, targetMixer, targetMixerPortIndex);
}
