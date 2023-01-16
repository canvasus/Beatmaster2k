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
const char * driver_names[5] = {"midi1", "midi2", "midi3", "midi4", "midi5"};
#define CNT_DEVICES (sizeof(midiDrivers)/sizeof(midiDrivers[0])) 
uint8_t launchPadMidiIndex = 0;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

const float DIV127 = (1.0/127.0);

void setupMidi()
{
  Serial.println(F("MIDI SETUP"));
  delay(1500);
  myusb.begin();
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleNoteOn(transposeMidiIn);
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
      if (*midiDrivers[index])
      {
        const uint8_t * productName = midiDrivers[index]->product();
        char buf[16];
        snprintf(buf, 16, "%s", productName);
        outputNames[index + 3] = buf;
        uint16_t productId = midiDrivers[index]->idProduct();
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
      delay(10);
    }
  }
  configureLaunchPad(LP_index);
  configureMidiInputDevices(LP_index);
}

void getUsbDeviceName(uint8_t usbIndex, char * buf, uint8_t maxBufferSize)
{
  char na[4] = {'N', '/', 'A'};
  if (*midiDrivers[usbIndex])
  {
    const uint8_t * productName = midiDrivers[usbIndex]->product();
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
  launchPadMidiIndex = driverIndex;
  LP1.setProgrammerMode();
}

//void setLaunchPadKeyboardMode(bool keyboardActive)
//{
//  if (keyboardActive)
//  {
//    midiDrivers[launchPadMidiIndex]->setHandleNoteOn(nullptr);
//    midiDrivers[launchPadMidiIndex]->setHandleNoteOff(nullptr);
//    midiDrivers[launchPadMidiIndex]->setHandleControlChange(nullptr);
//  }
//  else
//  {
//    midiDrivers[launchPadMidiIndex]->setHandleNoteOn(LPNoteOn);
//    midiDrivers[launchPadMidiIndex]->setHandleNoteOff(LPNoteOff);
//    midiDrivers[launchPadMidiIndex]->setHandleControlChange(LPControlChange);
//  }
//}

//char LPkeyboardPoll()
//{
//  char character = 0;
//  uint8_t index = 0;
//  if (midiDrivers[launchPadMidiIndex]->read() && (midiDrivers[launchPadMidiIndex]->getType() == 144) )
//  {
//    uint8_t data1 = midiDrivers[launchPadMidiIndex]->getData1();
//    uint8_t row = LPnoteToPadRow(data1);
//    uint8_t column = LPnoteToPadColumn(data1);
//    index = 56 - row * 8  + column;
//    
//    if (index < 26) character = index + 65;
//    if (index >= 26 && index <= 34) character = index + 22;
//    //Serial.printf("Index: %d, Char: %d\n", index, character);
//  }
//  
//  //Serial.printf("Type: %d, data1: %d, data2: %d\n", type, data1, data2);
//  return character;
//}

void configureMidiInputDevices(uint8_t LP_index)
{
   for (uint8_t driverIndex = 0; driverIndex < CNT_DEVICES; driverIndex++)
    {
      if (driverIndex != LP_index) midiDrivers[driverIndex]->setHandleNoteOn(transposeMidiIn);
    }
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

//void voiceNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
//{ 
//  //Voices[channel].noteOn(note, velocity);
//}
//void voiceNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
//{
//  //Voices[channel].noteOff(note, velocity);
//}

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
void midi5NoteOn(uint8_t channel, uint8_t noteValue, uint8_t velocity) { midi5.sendNoteOn(noteValue, 127, channel); }
void midi5NoteOff(uint8_t channel, uint8_t noteValue, uint8_t velocity) { midi5.sendNoteOff(noteValue, 127, channel); }

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
  for (uint8_t index = 0; index < CNT_DEVICES; index++)
  {
    if (*midiDrivers[index] && (index != launchPadMidiIndex)) midiDrivers[index]->sendRealTime(usbMIDI.Clock);
    midiDrivers[index]->send_now();
  }
}

void sendMidiStart()
{
  MIDI.sendStart();
  for (uint8_t index = 0; index < CNT_DEVICES; index++)
  {
    if (*midiDrivers[index]) midiDrivers[index]->sendRealTime(usbMIDI.Start);
    midiDrivers[index]->send_now();
  }
}

void sendMidiStop()
{
  MIDI.sendStop();
  for (uint8_t index = 0; index < CNT_DEVICES; index++)
  {
    if (*midiDrivers[index]) midiDrivers[index]->sendRealTime(usbMIDI.Stop);
  }
}

void LPNoteOn(byte channel, byte note, byte velocity)
{
  uint8_t padColumn = LPnoteToPadColumn(note) + LP1.page * 8;
  uint16_t tickTemp = padColumn * TICKS_PER_COLUMN;
  uint8_t lowerRow = tracks[currentTrack]->lowerRow;
  uint8_t padRow = LPnoteToPadRow(note);

  //TODO: Move all this to sequencer functions

  if ( ( LPdisplayMode == LPMODE_PATTERN ) && ( sequencerEditMode == MODE_PATTERNEDIT) && (velocity > 0) )
  {
    // Add or remove events
      uint8_t padState = 0;
      bool auditTrack = true; //sequencerState == STATE_STOPPED;

      if (tracks[currentTrack]->getEventsInTickInterval(tickTemp, tickTemp + TICKS_PER_COLUMN - 1 , lowerRow + padRow) == 0) // no matching events
      {
        tracks[currentTrack]->addEvent(tickTemp, lowerRow + padRow, tracks[currentTrack]->getTrackDefaultVelocity(), tracks[currentTrack]->getTrackDefaultNoteLengthTicks(), auditTrack);
        padState = tracks[currentTrack]->color;
      }
      else
      {
        tracks[currentTrack]->removeEvents(tickTemp, tickTemp + TICKS_PER_COLUMN -1, lowerRow + padRow);
        padState = LP_OFF;
      }
      LPcopy_updateSingleEvent_converter(note, padState);
   } 

  if ( ( LPdisplayMode == LPMODE_PATTERN ) && ( sequencerEditMode == MODE_EVENTEDIT ) && (velocity > 0) )
  {
    // Select existing events
    currentEvent = tracks[currentTrack]->getEventId(tickTemp, lowerRow + padRow);
  }

  if  ( ( LPdisplayMode == LPMODE_SCENE ) && (velocity > 0) )
  {
    // add / remove patterns
    bool inPatternRange =  (velocity > 0 && note >= 21 && note <= 88); // in pattern range
    if (inPatternRange)
    {
      uint8_t padColumn = LPnoteToPadColumn(note);
      uint8_t padRow = LPnoteToPadRow(note);
      uint8_t trackId = padRow - 1;
      uint8_t patternId = padColumn;
      if (sequencerState == STATE_STOPPED) tracks[trackId]-> setPatternId(patternId);
      if (sequencerState == STATE_RUNNING) tracks[trackId]-> cuePatternId(patternId);
      updateSceneConfiguration(currentScene);
      currentPattern = patternId;
      currentTrack = trackId;
    }
    bool inSceneRange =  (velocity > 0 && note >= 11 && note <= 18); // in scene range
    if (inSceneRange)
    {
      currentScene = LP1.page * 8 + note - 11;
      setScene(currentScene);
    }
  }

  if  ( ( LPdisplayMode == LPMODE_SONG ) && (velocity > 0) )
  {
    static uint8_t currentArrScene = 255;
    static uint8_t arrIndex = 0;

    bool inArrangementRange =  (velocity > 0 && note >= 21 && note <= 88); // in arrangement range
    if (inArrangementRange)
    {
      uint8_t row = LPnoteToPadRow(note);
      uint8_t column = LPnoteToPadColumn(note);
      arrIndex = 56 - row * 8  + column;
      if (sequencerEditMode != MODE_EVENTEDIT) SequencerData.arrangement[arrIndex] = currentArrScene;
      else
      {
        currentArrPosition = arrIndex;
        setSceneNr(SequencerData.arrangement[currentArrPosition]);        
      }
    }

    bool inSceneRange =  (velocity > 0 && note >= 11 && note <= 18); // in scene range
    if (inSceneRange)
    {
      currentArrScene = LP1.page * 8 + note - 11;
    }
  }
}

void LPNoteOff(byte channel, byte note, byte velocity)
{
  
}

void LPControlChange(byte channel, byte control, byte value)
{
  
  if (value > 0)
  {
    //Serial.printf("LP CC: %d, %d\n", control, value);
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
        if (LPdisplayMode == LPMODE_SCENE || LPdisplayMode == LPMODE_SONG) LPtoggleMute(trackTemp);
        else setCurrentTrack(trackTemp);
        break;
      case CCstartStop:
        if (sequencerState == STATE_STOPPED) startSequencer();
        else  stopSequencer();
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
      case CCsceneMode:
        setLPsceneMode();
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
