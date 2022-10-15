#include "audioFcns.h"

audioBackend::audioBackend()
{
  // voiceMixer -> -------------------- -> main out -> i2s + usb
  //            -> chorusSend -> chorus -^
  //            -> reverbSend -> reverb -^

  _connect(_voiceMixers[0], 0, _voiceMixers[2], 0);
  _connect(_voiceMixers[1], 0, _voiceMixers[2], 1);

  _connect(_voiceMixers[2], 0, _mainOutL, 0);  // dry channel L
  _connect(_voiceMixers[2], 0, _mainOutR, 0);  // dry channel R

  _connect(_voiceMixers[2], 0, _chorusSend, 0);  // chorus send (mono)
  _connect(_chorusSend, 0, _ensemble, 0);  // chorus in (mono)
  _connect(_ensemble, 0, _mainOutL, 1);  // chorus to main L
  _connect(_ensemble, 1, _mainOutR, 1);  // chorus to main R

  _connect(_voiceMixers[2], 0, _reverbSendL, 0);  // reverb send
  _connect(_voiceMixers[2], 0, _reverbSendR, 0);  // reverb send
  _connect(_reverbSendL, 0, _reverb, 0);  // reverb in L
  _connect(_reverbSendR, 0, _reverb, 1);  // reverb in R
  _connect(_reverb, 0, _mainOutL, 2);  // reverb to main L
  _connect(_reverb, 1, _mainOutR, 2);  // reverb to main R
   
  _connect(_mainOutL, 0, _audioOutput1, 0);
  _connect(_mainOutR, 0, _audioOutput1, 1);
  _connect(_mainOutL, 0, _audioOutput1, 2);
  _connect(_mainOutR, 0, _audioOutput1, 3);
  
  _connect(_mainOutL, 0, _usbGainL, 0);
  _connect(_mainOutR, 0, _usbGainR, 0);
  _connect(_usbGainL, 0, _audioOutputUSB, 0);
  _connect(_usbGainR, 0, _audioOutputUSB, 1);

  _mainOutL.gain(0, _parameters.mainOut_dry);
  _mainOutR.gain(0, _parameters.mainOut_dry);
  _mainOutL.gain(1, _parameters.mainOut_chorus);
  _mainOutR.gain(1, _parameters.mainOut_chorus);
  _mainOutL.gain(2, _parameters.mainOut_reverb);
  _mainOutR.gain(2, _parameters.mainOut_reverb);

  _chorusSend.gain(_parameters.chorusSendGain);
  
  _usbGainR.gain(_parameters.usbGain);
  _usbGainL.gain(_parameters.usbGain);

  _ensemble.lfoRate(_parameters.chorus_lfoRate);
  
  _reverb.size(_parameters.reverb_size);
  _reverb.hidamp(_parameters.reverb_hidamp);
  _reverb.lodamp(_parameters.reverb_lodamp);
  _reverb.lowpass(_parameters.reverb_lowpass);
  _reverb.diffusion(_parameters.reverb_diffusion);
 
}

void audioBackend::_connect(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput)
{
  _patchCords[_internalConnectionIndex] = new AudioConnection(source, sourceOutput, destination , destinationInput);
  _internalConnectionIndex++;
  if (_internalConnectionIndex > NR_PATCHCORDS - 1)
  {
    _internalConnectionIndex = NR_PATCHCORDS - 1;
    Serial.println("ERROR: Audio backend - no available patchcords");
  }
}

void audioBackend::setupAudio()
{
  Serial.println("AUDIO SETUP");
  AudioMemory(128);
  codecControl1.setAddress(0x1A);
  codecControl2.setAddress(0x1B);
  if (!codecControl1.enable()) Serial.println("Codec at 0x1A not responding");
  if (!codecControl2.enable()) Serial.println("Codec at 0x1B not responding");
  //_sgtl5000.enable();
  //_sgtl5000.volume(_parameters.hpVolume);
  for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++)
  {
    if (voiceIndex < 4)
    {
      Voices[voiceIndex].connectToMixer(_voiceMixers[0], voiceIndex);
      _voiceMixers[0].gain(voiceIndex, _parameters.voiceMixerGain);
      _voiceMixers[2].gain(voiceIndex, _parameters.voiceMixerGain);
    }
    if (voiceIndex >= 4 && voiceIndex < 8)
    {
      Voices[voiceIndex].connectToMixer(_voiceMixers[1], voiceIndex - 4);
      _voiceMixers[1].gain(voiceIndex - 4, _parameters.voiceMixerGain);
    }
  }
}

float audioBackend::getDryLevel() { return _parameters.mainOut_dry; }
float audioBackend::getChorusLevel() { return _parameters.mainOut_chorus; }
float audioBackend::getReverbLevel() { return _parameters.mainOut_reverb; }
void audioBackend::setDryLevel(float level)
{
  _parameters.mainOut_dry = level;
  _mainOutL.gain(0, _parameters.mainOut_dry);
  _mainOutR.gain(0, _parameters.mainOut_dry);
}

void audioBackend::setChorusLevel(float level)
{
  _parameters.mainOut_chorus = level;
  _mainOutL.gain(1, _parameters.mainOut_chorus);
  _mainOutR.gain(1, _parameters.mainOut_chorus);
}

void audioBackend::setReverbLevel(float level)
{
  _parameters.mainOut_reverb =  level;
  _mainOutL.gain(2, _parameters.mainOut_reverb);
  _mainOutR.gain(2, _parameters.mainOut_reverb);
}

// HELPER FUNCTIONS FOR UI
// **************************
void setDryLevel(float level) { AudioBackend.setDryLevel(level); }
void setChorusLevel(float level) { AudioBackend.setChorusLevel(level); }
void setReverbLevel(float level) { AudioBackend.setReverbLevel(level); }
float getDryLevel() { return AudioBackend.getDryLevel(); }
float getChorusLevel() { return AudioBackend.getChorusLevel(); }
float getReverbLevel() { return AudioBackend.getReverbLevel(); }

void displayResources()
{
  Serial.print("Audio mem usage: ");
  Serial.println(AudioMemoryUsageMax());
  Serial.print("Audio processor usage: ");
  Serial.println(AudioProcessorUsageMax());
  AudioMemoryUsageMaxReset(); 
  AudioProcessorUsageMaxReset(); 
}
