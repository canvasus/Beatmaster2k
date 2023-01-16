#include "audioFunctions.h"

SM2k_AudioControlWM8731 codecControl1;
//SM2k_AudioControlWM8731 codecControl2;

// GUItool: begin automatically generated code
AudioPlayMemory          player[NR_PLAYERS];
AudioMixer4              playerMixL1; 
AudioMixer4              playerMixL2;
AudioMixer4              playerMixR1;
AudioMixer4              playerMixR2;
AudioMixer4              playerMixL3;
AudioMixer4              playerMixR3;
AudioEffectDynamics      dynamicsL;
AudioEffectDynamics      dynamicsR;
AudioEffectPlateReverb   reverb;

AudioMixer4              finalMixL;
AudioMixer4              finalMixR;

AudioOutputI2S           i2s1;
//AudioOutputI2SQuad       i2s_quad1;

AudioConnection          patchCord1(player[0], 0, playerMixL1, 0);
AudioConnection          patchCord2(player[0], 0, playerMixR1, 0);
AudioConnection          patchCord3(player[1], 0, playerMixL1, 1);
AudioConnection          patchCord4(player[1], 0, playerMixR1, 1);
AudioConnection          patchCord5(player[2], 0, playerMixL1, 2);
AudioConnection          patchCord6(player[2], 0, playerMixR1, 2);
AudioConnection          patchCord7(player[3], 0, playerMixL1, 3);
AudioConnection          patchCord8(player[3], 0, playerMixR1, 3);

AudioConnection          patchCord9(player[4], 0, playerMixL2, 0);
AudioConnection          patchCord10(player[4], 0, playerMixR2, 0);
AudioConnection          patchCord11(player[5], 0, playerMixL2, 1);
AudioConnection          patchCord12(player[5], 0, playerMixR2, 1);
AudioConnection          patchCord13(player[6], 0, playerMixL2, 2);
AudioConnection          patchCord14(player[6], 0, playerMixR2, 2);
AudioConnection          patchCord15(player[7], 0, playerMixL2, 3);
AudioConnection          patchCord16(player[7], 0, playerMixR2, 3);

AudioConnection          patchCord17(playerMixL1, 0, playerMixL3, 0);
AudioConnection          patchCord18(playerMixL2, 0, playerMixL3, 1);
AudioConnection          patchCord19(playerMixR1, 0, playerMixR3, 0);
AudioConnection          patchCord20(playerMixR2, 0, playerMixR3, 1);

AudioConnection          patchCord21(playerMixL3, 0, dynamicsL, 0);
AudioConnection          patchCord22(playerMixR3, 0, dynamicsR, 0);

AudioConnection          patchCord23(dynamicsL, 0, finalMixL, 0); // dry L
AudioConnection          patchCord24(dynamicsR, 0, finalMixR, 0); // dry R

AudioConnection          patchCord25(dynamicsL, 0, reverb, 0); // reverb send L
AudioConnection          patchCord26(dynamicsR, 0, reverb, 1); // reverb send R

AudioConnection          patchCord27(reverb, 0, finalMixL, 1); // reverb return L
AudioConnection          patchCord28(reverb, 1, finalMixR, 1); // reverb return R

AudioConnection          patchCord29(finalMixL, 0, i2s1, 0);
AudioConnection          patchCord30(finalMixR, 0, i2s1, 1);


String sampleNames[NR_SAMPLES] = {"Bd Linn", "Sd Linn", "Hh Mpc60"};

const unsigned int *  samples[NR_SAMPLES] = {
                                             AudioSampleBd_linn,
                                             AudioSampleSd_linn,
                                             AudioSampleHh_rytmmpc60_2,
                                             };

drumKitParameters DrumKitParameters;

void setupAudio()
{
  Serial.println(F("AUDIO SETUP"));
  AudioMemory(196);
 
  codecControl1.setAddress(0x1A);
  //codecControl2.setAddress(0x1B);
  codecControl1.enable();
  //codecControl2.enable();

  reverb.size(DrumKitParameters.reverb_size);
  reverb.hidamp(DrumKitParameters.reverb_hidamp);
  reverb.lodamp(DrumKitParameters.reverb_lodamp);
  reverb.lowpass(DrumKitParameters.reverb_lowpass);
  reverb.diffusion(DrumKitParameters.reverb_diffusion);

  finalMixL.gain(0, DrumKitParameters.mainOut_dry);
  finalMixR.gain(0, DrumKitParameters.mainOut_dry);

  finalMixL.gain(1, DrumKitParameters.mainOut_reverb);
  finalMixR.gain(1, DrumKitParameters.mainOut_reverb);
  
  dynamicsL.compression(DrumKitParameters.dynThreshold, DrumKitParameters.dynAttack,
                          DrumKitParameters.dynRelease, DrumKitParameters.dynRatio,
                          DrumKitParameters.dynKneeWidth, DrumKitParameters.dynInGain);
  
  dynamicsR.compression(DrumKitParameters.dynThreshold, DrumKitParameters.dynAttack,
                          DrumKitParameters.dynRelease, DrumKitParameters.dynRatio,
                          DrumKitParameters.dynKneeWidth, DrumKitParameters.dynInGain);
  
}

void voiceNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{ 
  uint8_t playerId = constrain(note, 0, NR_PLAYERS - 1);
  uint8_t sampleId = DrumKitParameters.sampleId[playerId];
  player[playerId].play(samples[sampleId]);
}

void voiceNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  
}

void setPlayerLevel(uint8_t playerId, float level)
{
  
}

void setPlayerPanning(uint8_t playerId, float pan)
{
  
}
