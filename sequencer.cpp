#include "sequencer.h"
#include "track.h"
#include "midiFunctions.h"

uint8_t sequencerState      = STATE_STOPPED;
uint8_t columnTimer         = 0;
elapsedMicros tickTimer     = 0;
uint16_t   bpm               = 120;
uint16_t  oneTickUs         = 1000 * 60000 / (bpm * ticksPerBeat);
int sequencerStep = -1;

uint8_t sequencerEditMode = MODE_PATTERNEDIT;

IntervalTimer sequencerUpdateTimer;

MIDIcallback outputNoteOnFunctions[] = {voiceNoteOn, serialMidiNoteOn, deviceNoteOn, midi1NoteOn, midi2NoteOn, midi3NoteOn, midi4NoteOn};
MIDIcallback outputNoteOffFunctions[] = {voiceNoteOff, serialMidiNoteOff, deviceNoteOff, midi1NoteOff, midi2NoteOff, midi3NoteOff, midi4NoteOff};
String outputNames[] = {"Voice", "Serial", "USB device", "USB 1", "USB 2", "USB 3", "USB 4"};
uint8_t nrOutputFunctions = sizeof(outputNoteOnFunctions)/sizeof(outputNoteOnFunctions[0]);
const String noYesSelected[] = {"No", "Yes", "Selected"};

uint8_t arrangement[NR_TRACKS][NR_PATTERNS_IN_ARRANGEMENT];

void initSequencer()
{
  for (uint8_t trackId = 0;trackId < NR_TRACKS; trackId++)
  {
    for (uint8_t arrIndex = 0; arrIndex < NR_PATTERNS_IN_ARRANGEMENT; arrIndex++) arrangement[trackId][arrIndex] = ARR_NO_PATTERN;
  }
  
  //sequencerUpdateTimer.begin(updateSequencer, oneTickUs); 
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

void updateSequencer()
{
   for (uint8_t trackId = 0;trackId < NR_TRACKS; trackId++) tracks[trackId]->update();
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
  bpm = (uint16_t)newBpm;
  oneTickUs = 1000 * 60000 / (bpm * ticksPerBeat);
  sequencerUpdateTimer.update(oneTickUs); 
}

float getBpm() { return bpm; }

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
String getOutputEnum(uint8_t value) { return outputNames[value]; }

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
