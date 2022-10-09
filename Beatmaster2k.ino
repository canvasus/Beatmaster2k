#include "audioFcns.h"
#include "midiFunctions.h"
#include "ui.h"

audioBackend AudioBackend;

//IntervalTimer trackUpdateTimer;
//uint16_t interval =  1000000;

void setup()
{
  Serial.println("START");
  if (CrashReport) Serial.print(CrashReport);
  setupUI();
  delay(2000);
  AudioBackend.setupAudio();
  setupMidi();
  //trackUpdateTimer.begin(timerFunction, 1000000);
 
}

elapsedMillis beatTimer = 0;
uint8_t beatCounter4ths = 0;
uint8_t barCounter = 0;
elapsedMillis uiTimer = 0;
uint8_t note = 28;
int8_t barTranspose[4] = {0, 2, -4, 7};

void loop()
{
  updateMidi();
  if (beatTimer > 250)
  {
    beatTimer = 0; 
    beatCounter4ths++;
    if (beatCounter4ths > 3)
    {
      beatCounter4ths = 0;
      barCounter++;
      if (barCounter > 3) barCounter = 0;
    }
    doBeat();
  }
}

void doBeat()
{
  static uint8_t lastPad = 11;
  LP1.setPadColor(lastPad, LP_OFF);
  LP1.setPadColor(11 + barCounter, LP_RED);
  lastPad = 11 + barCounter;
  updateDisplayUI();
  uint8_t doNote = note + barTranspose[barCounter];
  switch (beatCounter4ths)
    {
      case 0:
        Voices[0].noteOn(doNote,127); 
        Voices[1].noteOn(doNote,127); 
        Voices[0].triggerSideChain(1);
        break;
      case 1:
        Voices[0].noteOn(doNote + 12,127); 
        break;
      case 2:
        Voices[0].noteOn(doNote,127);
        Voices[1].noteOn(doNote,127); 
        Voices[0].triggerSideChain(1);
        break;
      case 3:
        Voices[0].noteOn(doNote + 12,127);
        break;
    }
}
