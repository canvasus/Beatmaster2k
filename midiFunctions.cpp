#include "midiFunctions.h"

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice_BigBuffer midi1(myusb); // Launchpad or Midi device
MIDIDevice_BigBuffer midi2(myusb); // Launchpad or Midi device
MIDIDevice_BigBuffer midi3(myusb); // Launchpad or Midi device
MIDIDevice_BigBuffer midi4(myusb); // Launchpad or Midi device
LaunchPad LP1 = LaunchPad(1);

USBDriver *drivers[] = {&midi1, &midi2, &midi3, &midi4};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0])) 
bool driver_active[CNT_DEVICES] = {false, false, false, false};
const char * driver_names[CNT_DEVICES] = {"midi1", "midi2", "midi3", "midi4"};

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

const float DIV127 = (1.0/127.0);

Track track0(0, "T0", LP_BLUE);
Track track1(1, "T1", LP_CYAN);
Track track2(2, "T2", LP_PURPLE);
Track track3(3, "T3", LP_PINK);
Track track4(4, "T4", LP_ORANGE);
const uint8_t nrTracks = 5;

Track *tracks[nrTracks] = {&track0, &track1, &track2, &track3, &track4};

voice Voices[] = {voice(0), voice(1), voice(2), voice(3), voice(4), voice(5), voice(6), voice(7)};

void setupMidi()
{
  Serial.println("MIDI SETUP");
  delay(1000);
  myusb.begin();
  delay(100);
  
  //usbMIDI.setHandleNoteOn(deviceNoteOn);
  //usbMIDI.setHandleNoteOff(deviceNoteOff);
  //usbMIDI.setHandleControlChange(myControlChange);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  //MIDI.setHandleNoteOn(deviceNoteOn);
  //MIDI.setHandleNoteOff(deviceNoteOff);
  //MIDI.setHandleControlChange(myControlChange);
  locateUsbComponents();
  LPinit();
}

void locateUsbComponents()
{
  bool LPassigned = false;
  bool localKeysAssigned = true;
  uint8_t LP_index = 0;
  //uint8_t localKeys_index = 0;
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

void getUsbDeviceName(uint8_t usbIndex, char * buf, uint8_t maxBufferSize)
{
  char na[4] = {'N', '/', 'A'};
  if (*drivers[usbIndex])
  {
    const uint8_t * productName = drivers[usbIndex]->product();
    for(uint8_t i = 0; i < maxBufferSize; i++) buf[i] = productName[i];
  }
  else
  {
    for(uint8_t i = 0; i < 4; i++) buf[i] = na[i];
  }
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
      midi1.setHandleControlChange(LPControlChange);
      Serial.println("Initializing LP @ port midi1");
      break;
    case 1: // midi2
      LP1.begin(&midi2);
      midi2.setHandleNoteOn(LPNoteOn);
      midi2.setHandleNoteOff(LPNoteOff);  
      midi2.setHandleControlChange(LPControlChange);
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
  Serial.print("Device send note ON: ");
  Serial.println(note, DEC);
  //usbMIDI.sendNoteOn(note, velocity, channel);
  
}

void deviceNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  //usbMIDI.sendNoteOff(note, velocity, channel);
}

void voiceNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) { Voices[channel].noteOn(note, velocity); }
void voiceNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) { Voices[channel].noteOff(note, velocity); }

void serialMidiNoteOn(uint8_t channel, uint8_t noteValue, uint8_t velocity) { MIDI.sendNoteOn(noteValue, 127, channel); }
void serialMidiNoteOff(uint8_t channel, uint8_t noteValue, uint8_t velocity) { MIDI.sendNoteOff(noteValue, 127, channel); }

void midi1NoteOn(uint8_t channel, uint8_t noteValue, uint8_t velocity) { midi1.sendNoteOn(noteValue, 127, channel); }
void midi1NoteOff(uint8_t channel, uint8_t noteValue, uint8_t velocity) { midi1.sendNoteOff(noteValue, 127, channel); }
void midi2NoteOn(uint8_t channel, uint8_t noteValue, uint8_t velocity) { midi2.sendNoteOn(noteValue, 127, channel); }
void midi2NoteOff(uint8_t channel, uint8_t noteValue, uint8_t velocity) { midi2.sendNoteOff(noteValue, 127, channel); }
void midi3NoteOn(uint8_t channel, uint8_t noteValue, uint8_t velocity) { midi3.sendNoteOn(noteValue, 127, channel); }
void midi3NoteOff(uint8_t channel, uint8_t noteValue, uint8_t velocity) { midi3.sendNoteOff(noteValue, 127, channel); }
void midi4NoteOn(uint8_t channel, uint8_t noteValue, uint8_t velocity) { midi4.sendNoteOn(noteValue, 127, channel); }
void midi4NoteOff(uint8_t channel, uint8_t noteValue, uint8_t velocity) { midi4.sendNoteOff(noteValue, 127, channel); }

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
  //Serial.print("LP note ON: ");
  //Serial.println(note, DEC);
  
  if ( ( LPdisplayMode == DISPLAYMODE_SEQUENCER ) && ( sequencerEditMode == MODE_PATTERNEDIT ) )
  {
     if (velocity > 0) // Launchpad Mini sends velocity 127 at note on and 0 at off
    {
      uint8_t ticksPerColumn =  24 / LP1.xZoomLevel;
      uint8_t lowerRow = tracks[currentTrack]->lowerRow;
      uint8_t padColumn = LPnoteToPadColumn(note) + LP1.page * 8;
      uint8_t padRow = LPnoteToPadRow(note);
      uint8_t padState = 0;
      uint16_t tickTemp = padColumn * ticksPerColumn;
      bool auditTrack = true; //sequencerState == STATE_STOPPED;

      if (tracks[currentTrack]->getEventsInTickInterval(tickTemp, tickTemp + ticksPerColumn - 1 , lowerRow + padRow) == 0) // no matching events
      {
        tracks[currentTrack]->addEvent(tickTemp, lowerRow + padRow, 0, tracks[currentTrack]->getTrackDefaultNoteLengthTicks(), auditTrack);
        padState = tracks[currentTrack]->color;
      }
      else
      {
        tracks[currentTrack]->removeEvents(tickTemp, tickTemp + ticksPerColumn -1, lowerRow + padRow);
        padState = LP_OFF;
      }
      LP1.setPadColor(note, padState);
    }
  } // end, if DISPLAYMODE_SEQUENCER


  
//
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
  //Serial.print("LP note OFF: ");
  //Serial.println(note, DEC);
//  if (LPdisplayMode == DISPLAYMODE_PATTERNSELECT)
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

void LPControlChange(byte channel, byte control, byte value)
{
  //Serial.printf("LP CC: %d, %d\n", control, value);
  if (value > 0)
  {
    switch (control)
    {
      case CCtrack1:
      case CCtrack2:
      case CCtrack3:
      case CCtrack4:
      case CCtrack5:
        currentTrack = (control - 9) / 10 - 2;
         break;
      case CCstartStop:
        // toggle sequencer state stopped/running
        if (sequencerState == STATE_STOPPED)
        {
          startSequencer();
          LP1.setPadColor(CCstartStop, LP_GREEN);
          LPsetPageFromTrackData();
        }
        else
        {
          stopSequencer();
          LP1.setPadColor(CCstartStop, LP_RED);
        }
        break;
      case CCscrollUp:
        LP1.setPadColor(CCscrollUp, LP_GREEN);
        LPscrollUp();
        break;
      case CCscrollDown:
        LP1.setPadColor(CCscrollDown, LP_GREEN);
        LPscrollDown();
        break;
      case CCpageDecrease:
        LP1.setPadColor(CCpageDecrease, LP_GREEN);
        LPpageDecrease();
        break;
      case CCpageIncrease:
        LP1.setPadColor(CCpageIncrease, LP_GREEN);
        LPpageIncrease();
        break;
      case CCeditMode:
        // toggle pattern edit / event edit
        if (sequencerEditMode == MODE_PATTERNEDIT)
        {
          LP1.setPadColor(CCeditMode, LP_BLUE);
          sequencerEditMode = MODE_EVENTEDIT;
        }
        else
        {
          LP1.setPadColor(CCeditMode, LP_RED);
          sequencerEditMode = MODE_PATTERNEDIT;
        }

        break;
    }
  }
  if (value == 0)
  {
     switch (control)
    {
      case CCscrollUp:
        LP1.setPadColor(CCscrollUp, LP_OFF);
        break;
      case CCscrollDown:
        LP1.setPadColor(CCscrollDown, LP_OFF);
        break;
      case CCpageDecrease:
        LP1.setPadColor(CCpageDecrease, LP_OFF);
         break;
      case CCpageIncrease:
        LP1.setPadColor(CCpageIncrease, LP_OFF);
         break;
    }
  }
}
