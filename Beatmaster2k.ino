#include "audioFcns.h"
#include "midiFunctions.h"
#include "sequencer.h"
#include "ui.h"

audioBackend AudioBackend;

void setup()
{
  Serial.println("START");
  if (CrashReport) Serial.print(CrashReport);
  setupUI();
  AudioBackend.setupAudio();
  setupMidi();
  initSequencer();
}

elapsedMillis debugTimer = 0;
uint8_t counter = 0;
uint8_t notes[] = {58, 60, 67, 70};
uint8_t nr_notes = sizeof(notes);

void loop()
{
  //updateSequencer();
  updateMidi();
  updateUI();
//  if (debugTimer > 250)
//  {
//    debugTimer = 0;
//    Voices[0].noteOn(notes[counter], 0);
//    counter++;
//    if (counter > nr_notes - 1) counter = 0;
//  }
}
