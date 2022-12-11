#include "audioFcns.h"
#include "midiFunctions.h"
#include "sequencer.h"
#include "ui.h"

audioBackend AudioBackend;

void setup()
{
  Serial.println(F("START"));
  if (CrashReport) Serial.print(CrashReport);
  setupUI();
  AudioBackend.setupAudio();
  setupMidi();
  initSequencer();
}

void loop()
{
  updateMidi();
  updateUI();
  //updateSequencer();
}
