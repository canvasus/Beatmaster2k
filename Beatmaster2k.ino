#include "midiFunctions.h"
#include "audioFunctions.h"
#include "sequencer.h"
#include "ui.h"
#include "sdMgr.h"

void setup()
{
  Serial.println(F("START"));
  if (CrashReport) Serial.print(CrashReport);
  setupUI();
  setupMidi();
  setupAudio();
  initSDcard();
  initSequencer();
}

void loop()
{
  updateMidi();
  updateUI();
  updateSequencer();
}
