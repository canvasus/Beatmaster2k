#include "sequencer.h"

Track track0(0, "T0", LP_BLUE);
Track track1(1, "T1", LP_CYAN);
Track track2(2, "T2", LP_PURPLE);
Track track3(3, "T3", LP_PINK);
Track track4(4, "T4", LP_ORANGE);
Track track5(5, "T5", LP_GREEN);
Track track6(6, "T6", LP_YELLOW);
const uint8_t nrTracks = 7;
Track *tracks[nrTracks] = {&track0, &track1, &track2, &track3, &track4, &track5, &track6};

sequencerData SequencerData;

uint8_t sequencerState      = STATE_STOPPED;
uint8_t columnTimer         = 0;
elapsedMicros tickTimer     = 0;
uint16_t  oneTickUs         = 1000 * 60000 / (SequencerData.bpm * ticksPerBeat);
int sequencerStep = -1;
uint8_t sequencerEditMode = MODE_PATTERNEDIT;

IntervalTimer sequencerUpdateTimer;

MIDIcallback outputNoteOnFunctions[] = {voiceNoteOn, serialMidiNoteOn, deviceNoteOn, midi1NoteOn, midi2NoteOn, midi3NoteOn, midi4NoteOn, midi5NoteOn};
MIDIcallback outputNoteOffFunctions[] = {voiceNoteOff, serialMidiNoteOff, deviceNoteOff, midi1NoteOff, midi2NoteOff, midi3NoteOff, midi4NoteOff, midi5NoteOff};
String outputNames[] = {"Dummy", "Serial", "USB device", "USB 0", "USB 1", "USB 2", "USB 3", "USB 4"};
uint8_t nrOutputFunctions = sizeof(outputNoteOnFunctions)/sizeof(outputNoteOnFunctions[0]);
const String noYesSelected[] = {"No", "Yes", "Selected"};

void initSequencer()
{
  for (uint8_t sceneId = 0;sceneId < NR_SCENES; sceneId++) SequencerData.sceneColors[sceneId] = LP_GREEN;
  sequencerUpdateTimer.begin(tickTracks, oneTickUs); 
}

void tickTracks()
{
  if (sequencerState == STATE_RUNNING)
  {
    //AudioNoInterrupts(); 
    for (uint8_t trackId = 0;trackId < NR_TRACKS; trackId++) tracks[trackId]->tickTrack();
    //AudioInterrupts();
  }
}

void startSequencer()
{
   //sendMidiStart();
   sequencerState = STATE_RUNNING;
}

void stopSequencer()
{
  //sendMidiStop();
  sequencerState = STATE_STOPPED;
  sequencerStep = -1;
  resetTracks();
  flushTracksPlayedBuffers();
}

void setBpm(float newBpm)
{
  SequencerData.bpm = (uint16_t)newBpm;
  oneTickUs = 1000 * 60000 / (SequencerData.bpm * ticksPerBeat);
  sequencerUpdateTimer.update(oneTickUs); 
}

float getBpm() { return SequencerData.bpm; }

void setTrackOutput(float value)
{
  uint8_t outputIndex = (uint8_t)value;
  tracks[currentTrack]->setHandleNoteOn(outputNoteOnFunctions[outputIndex]);
  tracks[currentTrack]->setHandleNoteOff(outputNoteOffFunctions[outputIndex]);
  tracks[currentTrack]->outputId = outputIndex;
}

void setCurrentTrack(uint8_t track)
{
  currentTrack = track;
  LP1.page = 0; //min(LP1.page, maxPages);
}

float getTrackOutput() { return tracks[currentTrack]->outputId; }
String getOutputEnum(uint8_t value)
{
  if (value > 2 && value < nrOutputFunctions)
  {
    char buf[16];
    getUsbDeviceName(value - 3, buf, 16);
    outputNames[value] = buf;
  }
  return outputNames[value];
}

float getTrackChannel() { return (float)(tracks[currentTrack]->getTrackChannel()); }
void setTrackChannel(float channel) { tracks[currentTrack]->setTrackChannel((uint8_t)channel); }
String get0_16_note(uint8_t channel)
{
  if (channel < 17) return String(channel);
  else return "NoteVal";
}

float getTrackLengthColumns() { return (float)(tracks[currentTrack]->getPatternLengthColumns(LP1.xZoomLevel)); }
void setTrackLengthColumns(float columns) { tracks[currentTrack]->setPatternLengthColumns((uint16_t)columns, LP1.xZoomLevel); }

float getTrackDefLength() { return (float)(tracks[currentTrack]->getTrackDefaultNoteLengthTicks()); }
void setTrackDefLength(float length) { tracks[currentTrack]->setTrackDefaultNoteLengthTicks((uint16_t)length); }

float getTrackPatternNr() { return (float)(tracks[currentTrack]->getActivePatternId()); }

void setTrackPatternNr(float patternNr)
{
  if (sequencerState == STATE_STOPPED) tracks[currentTrack]-> setPatternId((uint8_t)patternNr);
  if (sequencerState == STATE_RUNNING) tracks[currentTrack]-> cuePatternId((uint8_t)patternNr);
  currentPattern = patternNr;
}

float getEventLength()
{
  TrackEvent tempEvent = tracks[currentTrack]->getEvent(currentPattern, currentEvent);
  return (float)(tempEvent.noteLength);
}

void setEventLength(float length)
{
  if (currentEvent != -1)
  {
    TrackEvent tempEvent = tracks[currentTrack]->getEvent(currentPattern, currentEvent);
    tempEvent.noteLength = (uint16_t)length;
    tracks[currentTrack]->setEvent(currentPattern, currentEvent, tempEvent);
  }
}

float getTransposeStatus() { return (float)tracks[currentTrack]->trackRespondToTranspose; }
void setTransposeStatus(float status) { tracks[currentTrack]->trackRespondToTranspose = (uint8_t)status; }
String getNoYesSelectedEnum(uint8_t value) { return noYesSelected[constrain(value, 0, 2)]; }

void setTranspose(int transpose)
{
  for (uint8_t trackId = 0;trackId < NR_TRACKS; trackId++)
  {
    if (tracks[trackId]->trackRespondToTranspose == 1) tracks[trackId]->transpose = transpose;
    if (tracks[trackId]->trackRespondToTranspose == 2 && trackId == currentTrack ) tracks[trackId]->transpose = transpose;
  }
}

void setPatternSpeed(float speed)
{
  uint8_t patternId = tracks[currentTrack]->getActivePatternId();
  tracks[currentTrack]->setPatternSpeed(patternId, (uint8_t)speed);
}

float getPatternSpeed()
{
  uint8_t patternId = tracks[currentTrack]->getActivePatternId();
  return (float)tracks[currentTrack]->getPatternSpeed(patternId);
}


String getPatternSpeedEnum(uint8_t value)
{
  switch (value)
  {
    case PATTERN_SPEED_x1_4: return "x1/4";
    case PATTERN_SPEED_x1_3: return "x1/3";
    case PATTERN_SPEED_x1_2: return "x1/2";
    case PATTERN_SPEED_x1: return "x1";
    case PATTERN_SPEED_x2: return "x2";
    default: return "ERR";
  }
}

void resetTracks()
{
  for (uint8_t trackId = 0;trackId < NR_TRACKS; trackId++) tracks[trackId]->loopResetTrack();
}

void flushTracksPlayedBuffers()
{
  for (uint8_t trackId = 0;trackId < NR_TRACKS; trackId++) tracks[trackId]->flushPlayedBuffer();
}

void updateSceneConfiguration(uint8_t sceneId)
{
  // update pattern configuration in selected scene
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
  {
    SequencerData.scenes[trackId][sceneId] =  tracks[trackId]->getActivePatternId();
  }
}

void setScene(uint8_t sceneId)
{
  // set or cue patterns from scene
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
  {
    if ( (sequencerState == STATE_STOPPED) && ( tracks[trackId]->getActivePatternId() != SequencerData.scenes[trackId][sceneId]) ) tracks[trackId]->setPatternId(SequencerData.scenes[trackId][sceneId]);
    if (sequencerState == STATE_RUNNING && ( tracks[trackId]->getActivePatternId() != SequencerData.scenes[trackId][sceneId]) ) tracks[trackId]->cuePatternId(SequencerData.scenes[trackId][sceneId]);
  }
}

float getSceneNr() { return (float)currentScene; }

void setSceneNr(float sceneId)
{
  currentScene = (uint8_t)sceneId;
  setScene(sceneId);
}

float getSceneColor() { return SequencerData.sceneColors[currentScene]; }
void setSceneColor(float color)
{
  SequencerData.sceneColors[currentScene] = (uint8_t)color;
  LPsetSceneButtonsSongMode(true);
}
