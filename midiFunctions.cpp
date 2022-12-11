#include "midiFunctions.h"

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice_BigBuffer midi1(myusb); // Launchpad or Midi device
MIDIDevice_BigBuffer midi2(myusb); // Launchpad or Midi device
MIDIDevice_BigBuffer midi3(myusb); // Launchpad or Midi device
MIDIDevice_BigBuffer midi4(myusb); // Launchpad or Midi device
MIDIDevice_BigBuffer midi5(myusb); // Launchpad or Midi device
LaunchPad LP1 = LaunchPad(1);

MIDIDevice_BigBuffer * midiDrivers[5] = {&midi1, &midi2, &midi3, &midi4, &midi5};
USBDriver *drivers[] = {&midi1, &midi2, &midi3, &midi4, &midi5};
const char * driver_names[5] = {"midi1", "midi2", "midi3", "midi4", "midi5"};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0])) 

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

const float DIV127 = (1.0/127.0);

Track track0(0, "T0", LP_BLUE);
Track track1(1, "T1", LP_CYAN);
Track track2(2, "T2", LP_PURPLE);
Track track3(3, "T3", LP_PINK);
Track track4(4, "T4", LP_ORANGE);
Track track5(5, "T5", LP_GREEN);
Track track6(6, "T6", LP_YELLOW);
const uint8_t nrTracks = 7;

Track *tracks[nrTracks] = {&track0, &track1, &track2, &track3, &track4, &track5, &track6};

//voice Voices[NR_VOICES] = {voice(0), voice(1), voice(2), voice(3)};
voice Voices[NR_VOICES] = {voice(0)};

void setupMidi()
{
  Serial.println(F("MIDI SETUP"));
  delay(1000);
  myusb.begin();
  delay(100);
  //usbMIDI.setHandleNoteOn(transposeMidiIn);
  //usbMIDI.setHandleNoteOff(transposeMidiIn);
  //usbMIDI.setHandleControlChange(myControlChange);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleNoteOn(transposeMidiIn);
  //MIDI.setHandleNoteOff(deviceNoteOff);
  //MIDI.setHandleControlChange(myControlChange);
  midi1.setHandleNoteOn(transposeMidiIn);
  midi2.setHandleNoteOn(transposeMidiIn);
  midi3.setHandleNoteOn(transposeMidiIn);
  midi4.setHandleNoteOn(transposeMidiIn);
  midi5.setHandleNoteOn(transposeMidiIn);
  locateUsbComponents();
  LPinit();
}

void locateUsbComponents()
{
  bool LPassigned = false;
  uint8_t LP_index = 0;
  Serial.println(F("Searching for usb components..."));
  while (! (LPassigned == true))
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

void configureLaunchPad(uint8_t driverIndex)
{
  LP1.begin(midiDrivers[driverIndex]);
  midiDrivers[driverIndex]->setHandleNoteOn(LPNoteOn);
  midiDrivers[driverIndex]->setHandleNoteOff(LPNoteOff);
  midiDrivers[driverIndex]->setHandleControlChange(LPControlChange);
  Serial.print(F("Initializing LP @ port: "));
  Serial.println(driver_names[driverIndex]);
  LP1.setProgrammerMode();
}

void transposeMidiIn(uint8_t channel, uint8_t note, uint8_t velocity) { setTranspose(note - 36); }

void deviceNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
  //Serial.printf("Device send, type %d, channel %d, note %d, velocity %d\n", usbMIDI.NoteOn, channel, note, velocity);
  usbMIDI.sendNoteOn(note, velocity, channel);
}

void deviceNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  usbMIDI.sendNoteOff(note, 127, channel);
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
  midi3.read();
  midi4.read();
  usbMIDI.read();
  MIDI.read();
}

void sendMidiClock()
{
  MIDI.sendClock();
  //midi1.sendRealTime(usbMIDI.Clock);
  //midi2.sendRealTime(usbMIDI.Clock);
  //midi3.sendRealTime(usbMIDI.Clock);
  //midi4.sendRealTime(usbMIDI.Clock);
}

void sendMidiStart()
{
  MIDI.sendStart();
}

void sendMidiStop()
{
  MIDI.sendStop();
}


void LPNoteOn(byte channel, byte note, byte velocity)
{
  //Serial.print("LP note ON: ");
  //Serial.println(note, DEC);
  uint8_t padColumn = LPnoteToPadColumn(note) + LP1.page * 8;
  uint8_t ticksPerColumn =  24 / LP1.xZoomLevel;
  uint16_t tickTemp = padColumn * ticksPerColumn;
  uint8_t lowerRow = tracks[currentTrack]->lowerRow;
  uint8_t padRow = LPnoteToPadRow(note);
   
  if ( ( LPdisplayMode == LPMODE_PATTERN ) && ( sequencerEditMode == MODE_PATTERNEDIT) && (velocity > 0) )
  {
    // Add or remove events
      uint8_t padState = 0;
      bool auditTrack = true; //sequencerState == STATE_STOPPED;

      if (tracks[currentTrack]->getEventsInTickInterval(tickTemp, tickTemp + ticksPerColumn - 1 , lowerRow + padRow) == 0) // no matching events
      {
        tracks[currentTrack]->addEvent(tickTemp, lowerRow + padRow, tracks[currentTrack]->getTrackDefaultVelocity(), tracks[currentTrack]->getTrackDefaultNoteLengthTicks(), auditTrack);
        padState = tracks[currentTrack]->color;
      }
      else
      {
        tracks[currentTrack]->removeEvents(tickTemp, tickTemp + ticksPerColumn -1, lowerRow + padRow);
        padState = LP_OFF;
      }
      LPcopy_updateSingleEvent_converter(note, padState);
   } 

  if ( ( LPdisplayMode == LPMODE_PATTERN ) && ( sequencerEditMode == MODE_EVENTEDIT ) && (velocity > 0) )
  {
    // Select existing events
    currentEvent = tracks[currentTrack]->getEventId(tickTemp, lowerRow + padRow);
  }

  if  ( ( LPdisplayMode == LPMODE_SONG ) && ( sequencerEditMode == MODE_PATTERNEDIT ) && (velocity > 0) )
  {
    // add / remove patterns
  }
}

  
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
//}

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
    uint8_t trackTemp = (control - 9) / 10 - 2;
    switch (control)
    {
      case CCtrack1:
      case CCtrack2:
      case CCtrack3:
      case CCtrack4:
      case CCtrack5:
      case CCtrack6:
      case CCtrack7:
        if (LPdisplayMode == LPMODE_SONG) LPtoggleMute(trackTemp);
        else setCurrentTrack(trackTemp);
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
        // toggle between add/delete and edit modes
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
      case CCsongMode:
        setLPsongMode();
        break;
      case CCpatternMode:
        setLPpatternMode();
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
