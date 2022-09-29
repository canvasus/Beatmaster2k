#include "audioFunctions.h"
#include "midiFunctions.h"
//#include "ui.h"

audioBackend AudioBackend;

void setup()
{
  Serial.println("START");
  if (CrashReport) Serial.print(CrashReport);
  AudioBackend.setupAudio();
  setupMidi();
  //setupUI();
}

elapsedMillis debugTimer = 0;
elapsedMillis uiTimer = 0;

void loop()
{
  updateMidi();
  if (uiTimer > 20)
  {
    uiTimer = 0;
    //Voices[0].triggerSideChain();
    Serial.println(Voices[0].getSCdata());
//    updateUI();
  }
}
