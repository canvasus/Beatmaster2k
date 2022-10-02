#include "audioFcns.h"
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
float notes[2] = {24, 36};

void loop()
{
  updateMidi();
  if (debugTimer > 250)
  {
    static uint8_t counter = 0;
    debugTimer = 0;
    Voices[0].noteOn(notes[counter], 0);
    counter++;
    if (counter > 1) counter = 0;
  }
}
