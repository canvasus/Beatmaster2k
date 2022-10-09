#include "midiFunctions.h"
#include "track.h"

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice_BigBuffer midi1(myusb); //Launchpad?
MIDIDevice_BigBuffer midi2(myusb); // Midi keyboard ?
MIDIDevice_BigBuffer midi3(myusb); // Midi keyboard ?
LaunchPad LP1 = LaunchPad(1);

USBDriver *drivers[] = {&midi1, &midi2, &midi3};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0])) 
bool driver_active[CNT_DEVICES] = {false, false, false};
const char * driver_names[CNT_DEVICES] = {"midi1", "midi2", "midi3"};

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

const float DIV127 = (1.0/127.0);

Track drumTrack1(1, LP_BLUE);
Track drumTrack2(2, LP_CYAN);
Track midiTrack1(3, LP_PURPLE);
Track midiTrack2(4, LP_PINK);
Track midiTrack3(5, LP_ORANGE);
const uint8_t nrTracks = 5;

Track *tracks[nrTracks] = {&drumTrack1, &drumTrack2, &midiTrack1, &midiTrack2, &midiTrack3};
String trackNames[nrTracks] = {"D1", "D2", "M1", "M2", "M3"};

voice Voices[] = {voice(0), voice(1), voice(2), voice(3), voice(4), voice(5), voice(6), voice(7)};

void setupMidi()
{
  Serial.println("MIDI SETUP");
  myusb.begin();
  delay(100);
  
  usbMIDI.setHandleNoteOn(deviceNoteOn);
  usbMIDI.setHandleNoteOff(deviceNoteOff);
  //usbMIDI.setHandleControlChange(myControlChange);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleNoteOn(deviceNoteOn);
  MIDI.setHandleNoteOff(deviceNoteOff);
  //MIDI.setHandleControlChange(myControlChange);
  locateUsbComponents();
}

void locateUsbComponents()
{
  bool LPassigned = false;
  bool localKeysAssigned = true;
  uint8_t LP_index = 0;
  uint8_t localKeys_index = 0;
  Serial.println("Searching for usb components...");
  while (! (LPassigned == true && localKeysAssigned == true))
  {
    for (uint8_t index = 0; index < CNT_DEVICES; index++)
    {
      if (*drivers[index])
      {
        const uint8_t * productName = drivers[index]->product();
        uint16_t productId = drivers[index]->idProduct();
        Serial.print(driver_names[index]);
        Serial.print(": PID 0x");
        Serial.print(productId, HEX);
        Serial.printf(" %s\n", productName);
        //if (productId == 0x1111 || productId == 0x76)
        //{
        //  localKeys_index = index;
        //  localKeysAssigned = true;
        //}
        if (productId == 0x113)
        {
          LP_index = index;
          LPassigned = true;
        }
      }
      delay(100);
    }
  }
  configureLaunchPad(LP_index);
  //configureLocalKeys(localKeys_index);
}

void configureLocalKeys(uint8_t driverIndex)
{
   switch(driverIndex)
  {
    case 0: // midi1
      midi1.setHandleNoteOn(localKeysNoteOn);
      midi1.setHandleNoteOff(localKeysNoteOff);
      //midi1.setHandlePitchChange(localKeysPitchBend);
      //midi1.setHandleControlChange(localKeysControlChange);
      Serial.println("Initializing local keys @ port midi1");
      break;
    case 1: // midi2
      midi2.setHandleNoteOn(localKeysNoteOn);
      midi2.setHandleNoteOff(localKeysNoteOff);
      //midi2.setHandlePitchChange(localKeysPitchBend);
      //midi2.setHandleControlChange(localKeysControlChange);
      Serial.println("Initializing local keys @ port midi2");  
      break;
  }
}

void configureLaunchPad(uint8_t driverIndex)
{
  switch(driverIndex)
  {
    case 0: // midi1
      LP1.begin(&midi1);
      midi1.setHandleNoteOn(LPNoteOn);
      midi1.setHandleNoteOff(LPNoteOff);  
      midi1.setHandleControlChange(myControlChange);
      Serial.println("Initializing LP @ port midi1");
      break;
    case 1: // midi2
      LP1.begin(&midi2);
      midi2.setHandleNoteOn(LPNoteOn);
      midi2.setHandleNoteOff(LPNoteOff);  
      midi2.setHandleControlChange(myControlChange);
      Serial.println("Initializing LP @ port midi2");
      break;
  }
  LP1.setProgrammerMode();
}

void localKeysNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
  //Serial.print("Local keys note ON: ");
  //Serial.println(note, DEC);
  Voices[0].triggerSideChain(1);
  Voices[1].noteOn(note, velocity);

}

void localKeysNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  //Serial.print("Local keys note OFF: ");
  //Serial.println(note, DEC);
  Voices[1].noteOff(note, velocity);
}

void deviceNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
  //Serial.print("Note ON: ");
  //Serial.println(note, DEC);

}

void deviceNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  //Serial.print("Note OFF: ");
  //Serial.println(note, DEC);

}

void myControlChange(byte channel, byte control, byte value)
{
  switch (control)
  {
    case CC_MODWHEEL:
      break;
  }
}

void updateMidi()
{
  myusb.Task();
  midi1.read();
  midi2.read();
  usbMIDI.read();
  MIDI.read();
}

void LPNoteOn(byte channel, byte note, byte velocity)
{
//  if (displayMode == DISPLAYMODE_SEQUENCER)
//  {
//     if (velocity > 0) // Launchpad Mini sends velocity 127 at note on and 0 at off
//    {
//      uint8_t padColumn = LPnoteToPadColumn(note) + padPage * 8;
//      uint8_t padRow = LPnoteToPadRow(note);
//      uint8_t padState = 0;
//      uint16_t tickTemp = padColumn * ticksPerColumn;
//
//      if (currentTrack->getEventsInTickInterval(tickTemp, tickTemp + ticksPerColumn - 1 , lowerRow + padRow) == 0) // no matching events
//      {
//        currentTrack->addEvent(tickTemp, lowerRow + padRow, 0, currentTrack->getTrackDefaultNoteLengthTicks(), (sequencerState == STATE_STOPPED));
//        padState = currentTrack->color;
//      }
//      else
//      {
//        currentTrack->removeEvents(tickTemp, tickTemp + ticksPerColumn -1, lowerRow + padRow);
//        padState = LP_OFF;
//      }
//      LP1.setPadColor(note, padState);
//    }
//  } // end, if DISPLAYMODE_SEQUENCER
//
//  if (displayMode == DISPLAYMODE_MIXER)
//  {
//    if (velocity > 0)
//    {
//      uint8_t padColumn = LPnoteToPadColumn(note);
//      uint8_t padRow = LPnoteToPadRow(note);
//      float mixerValue = mapMixerUint8ToFloat(padColumn);
//      mixerMatrixDrum1[padRow] = mixerValue;
//      renderMixerPage();
//    }
//  }
//  if (displayMode == DISPLAYMODE_PATTERNSELECT)
//  {
//    bool inPatternRange =  (velocity > 0 && note >= 21 && note <= 68); // in pattern range
//    //if clear noteOn - > set clearArmed
//    //if clearArmed and in patternPad range --> execute clear
//    if (note == notePatternClear) clearRequestArmed = true;
//    if (note == notePatternCopy) copyRequestArmed = true;
//    if (copyRequestArmed && clearRequestArmed)
//    {
//      copyRequestArmed = false;
//      clearRequestArmed = false;
//    }
//    if (clearRequestArmed)
//    {
//      LP1.setPadColorFlashing(notePatternClear, LP_RED); // clear pattern flashing
//      renderNotification("CLEAR");
//    }
//    if (copyRequestArmed)
//    {
//      LP1.setPadColorFlashing(notePatternCopy, LP_ORANGE);
//      renderNotification("COPY");
//    }
//    
//    if (inPatternRange) //(velocity > 0 && note >= 21 && note <= 68) 
//    {
//      uint8_t padColumn = LPnoteToPadColumn(note);
//      uint8_t padRow = LPnoteToPadRow(note);
//      uint8_t trackId = padRow - 1;
//      uint8_t patternId = padColumn;
//
//      if (!clearRequestArmed && !copyRequestArmed)
//      {
//        if (sequencerState == STATE_STOPPED) tracks[trackId]-> setPatternId(patternId);
//        if (sequencerState == STATE_RUNNING) tracks[trackId]-> cuePatternId(patternId);
//        renderPatternPage();
//      }
//      
//      if (clearRequestArmed) // clear armed and note in range --> clear selected pattern
//      {
//        tracks[trackId]->clearPattern(patternId);
//      }
//
//      if (copyRequestArmed)
//      {
//        if (copyRequestSourceSelected)
//        {
//          copyPattern(copySourceTrackId, copySourcePatternId, trackId, patternId);
//          copySourceTrackId = 0;
//          copySourcePatternId = 0;
//          copyRequestSourceSelected = false;
//          copyRequestArmed = false;
//          LP1.setPadColor(notePatternCopy, LP_ORANGE);
//        }
//        else
//        {
//          copySourceTrackId = trackId;
//          copySourcePatternId = patternId;
//          copyRequestSourceSelected = true;
//        }
//      }
//    }
//  }
}

void LPNoteOff(byte channel, byte note, byte velocity)
{
//  if (displayMode == DISPLAYMODE_PATTERNSELECT)
//  {
//    switch (note)
//    {
//      case notePatternClear:
//        clearRequestArmed = false;
//        LP1.setPadColor(notePatternClear, LP_RED);
//        renderNotification("     ");
//        break;
//      case notePatternCopy:
//        copySourceTrackId = 0;
//        copySourcePatternId = 0;
//        copyRequestSourceSelected = false;
//        copyRequestArmed = false;
//        LP1.setPadColor(notePatternCopy, LP_ORANGE);
//        renderNotification("     ");
//        break;
//    }
//  }
}
