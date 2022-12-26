#include "midiFunctions.h"
#include "sequencer.h"
#include "ui.h"
#include "sdMgr.h"

void setup()
{
  Serial.println(F("START"));
  if (CrashReport) Serial.print(CrashReport);
  setupUI();
  setupMidi();
  initSDcard();
  initSequencer();
}

void loop()
{
  updateMidi();
  updateUI();
  updateSequencer();
}
