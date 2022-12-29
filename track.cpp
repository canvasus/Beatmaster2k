#include "track.h"
#include "midiFunctions.h"

Track::Track(uint8_t trackId, String trackName, uint8_t trackColor)
{
  // constructor code here
    name                  = trackName;
   _trackId               = trackId;
   _nrEvents              = NR_TRACK_EVENTS;
   _cuedEventIndex        = 0;
   _midiChannel           = trackId;
   _defaultNoteLengthTicks = 8 ; //(4 / 96 = <1/16)
   _defaultVelocity       = 127;
   
   _noteOn_cb = nullptr;
   _noteOff_cb = nullptr;
   outputId = 0;
   trackRespondToTranspose = false;
   trackMuted = false;
   _tickFlag = 0;
   _cuedMuteFlag =  false;
   _cuedMuteStatus =  false;

   _prescalerCounter = 0;

   for (int patternId = 0; patternId < NR_PATTERNS; patternId++)
   {
    for (int i = 0; i < _nrEvents;i++)
    {
      _patterns[patternId].trackEvents[i] = empty_trackEvent;
    }
    _patterns[patternId].patternId = patternId;
    _patterns[patternId].patternLengthTicks = 8 * RESOLUTION16TH - 1; //RESOLUTION - 1; // 4 * 1/4th, (previously set to _trackLengthTicks)
    _patterns[patternId].patternStatus = PATTERN_EMPTY;
    _patterns[patternId].patternSpeed = PATTERN_SPEED_x1;
   }

   for (int i = 0; i < NR_PLAYED_EVENTS;i++) { _playedEvents[i] = empty_trackEvent; }
  
   _currentTrackTick = 0;
   _currentPattern = 0;
   _cuedPattern = 255; // 255 = no pattern cued
   color = trackColor;
   transpose = 0;
   lowerRow = 0; // which UI row to set when switching to this track. 0 for drums and higher for midi
}

uint8_t Track::getId() { return _trackId; }

void Track::tickTrack() // sent from main sequencer at defined tick rate
{
  _tickFlag = _tickPrescaler();
  _currentTrackTick = _currentTrackTick + _tickFlag;
  
  if (_currentTrackTick > _patterns[_currentPattern].patternLengthTicks) //_trackLengthTicks
  {
    _currentTrackTick = 0;
    loopResetTrack();
    _tickFlag = 0;
  }
 if (!trackMuted) _triggerEvents();
  _handleAutoNoteOff();
}

uint16_t Track::_tickPrescaler()
{
  switch (_patterns[_currentPattern].patternSpeed)
  {
    case PATTERN_SPEED_x1: return 1;
    
    case PATTERN_SPEED_x2: return 2;
    
    case PATTERN_SPEED_x1_4:
      _prescalerCounter++;
      if (_prescalerCounter >= 4)
      {
        _prescalerCounter = 0;
        return 1;
      }
      else return 0;

    case PATTERN_SPEED_x1_3:
      _prescalerCounter++;
      if (_prescalerCounter >= 3)
      {
        _prescalerCounter = 0;
        return 1;
      }
      else return 0;
   
    case PATTERN_SPEED_x1_2:
      _prescalerCounter++;
      if (_prescalerCounter >= 2)
      {
        _prescalerCounter = 0;
        return 1;
      }
      else return 0;
    
    default: return 0;
  }
}

void Track::loopResetTrack()
{
  _cuedEventIndex = 0;
  _currentTrackTick = 0;
  // check if a pattern change is cued here?
  if (_cuedPattern != 255)
  {
    _currentPattern = _cuedPattern;
    _cuedPattern =  255;
    _compactEventArray();
  }
  if (_cuedMuteFlag)
  {
    trackMuted = _cuedMuteStatus;
    _cuedMuteFlag =  false;
  }
  //printEventArray();
}

void Track::cueMuteStatus(bool status)
{
  _cuedMuteStatus = status;
  _cuedMuteFlag = true;
}

uint8_t Track::getMuteStatus()
{
  if ( (trackMuted == true) && (_cuedMuteFlag == false) ) return MUTE_ON;
  if ( (trackMuted == false) && (_cuedMuteFlag == true) && (_cuedMuteStatus == true) ) return MUTE_ON_CUED;
  if ( (trackMuted == true) && (_cuedMuteFlag == true) && (_cuedMuteStatus == false) ) return MUTE_OFF_CUED;
  return MUTE_OFF;
}

void Track::_triggerEvents()
{
  // SEND ALL EVENTS with tick <= currentTick. Set status played. Copy event to "played" buffer
  uint16_t index = _cuedEventIndex;
  while((_patterns[_currentPattern].trackEvents[index].tick <= _currentTrackTick) && (_patterns[_currentPattern].trackEvents[index].header != NO_EVENT) && (index < _nrEvents))
  {
    if(_patterns[_currentPattern].trackEvents[index].header == EVENT_MIDI_SLEEPING) _patterns[_currentPattern].trackEvents[index].header = EVENT_MIDI_PLAYED;
    if(_noteOn_cb &&  (_patterns[_currentPattern].trackEvents[index].noteValue + transpose < 128))
    {
      //Serial.println("called midicb");
      if (_midiChannel == MIDICH_NOTEVALUE)
      {
        uint8_t noteValue = _patterns[_currentPattern].trackEvents[index].noteValue + transpose;
        uint8_t channel = constrain(noteValue, 1, 16);
        _noteOn_cb(channel, _patterns[_currentPattern].trackEvents[index].noteValue + transpose, _patterns[_currentPattern].trackEvents[index].noteVelocity);
        _copyToPlayedBuffer(_patterns[_currentPattern].trackEvents[index]);
      }
      else
      {
        _noteOn_cb(_midiChannel, _patterns[_currentPattern].trackEvents[index].noteValue + transpose, _patterns[_currentPattern].trackEvents[index].noteVelocity);
        _copyToPlayedBuffer(_patterns[_currentPattern].trackEvents[index]);
      }
    }
    index = index + 1;
    _updateCuedEventIndex();
  }
}

void Track::_copyToPlayedBuffer(trackEvent event)
{
  // note: store actual noteValue with transpose in played buffer
  event.noteValue = event.noteValue + transpose;
  uint8_t index = 0;
  while(_playedEvents[index].header != NO_EVENT && index < NR_PLAYED_EVENTS) index = index + 1;
  _playedEvents[index] = event;
}

void Track::_handleAutoNoteOff()
{
  // If events in played buffer have current tick >= event tick + event length, send note off
  // change to:
  // if length == 0 send note off
  // otherwise length = length - 1 tick * prescaler
  for (uint8_t index = 0; index < NR_PLAYED_EVENTS; index++)
  {
    if((_playedEvents[index].header == EVENT_MIDI_PLAYED) && (_playedEvents[index].noteLength == 0))
    {
      if(_noteOff_cb) _noteOff_cb(_midiChannel,_playedEvents[index].noteValue, 0);
      _playedEvents[index] = empty_trackEvent;
      //printPlayedArray();
    }
    else if (_playedEvents[index].header == EVENT_MIDI_PLAYED)
    {
      //_playedEvents[index].noteLength = _playedEvents[index].noteLength - 1;
      _playedEvents[index].noteLength = _playedEvents[index].noteLength - _tickFlag;
    }
  }
}

void Track::flushPlayedBuffer()
{
  //printPlayedArray();
  for (uint8_t index = 0; index < NR_PLAYED_EVENTS; index++)
  {
    if(_playedEvents[index].header == EVENT_MIDI_PLAYED)
    {
      if(_noteOff_cb) _noteOff_cb(_midiChannel,_playedEvents[index].noteValue, _playedEvents[index].noteVelocity);
      _playedEvents[index] = empty_trackEvent;
    }
  }
  //printPlayedArray();
}

uint16_t Track::getCurrentTrackTick() { return _currentTrackTick;}
void Track::setPatternLengthBeats(uint16_t lengthBeats) { _patterns[_currentPattern].patternLengthTicks = lengthBeats * RESOLUTION - 1; }
uint16_t Track::getPatternLengthBeats() { return ( (_patterns[_currentPattern].patternLengthTicks + 1) / RESOLUTION); }

uint16_t Track::getPatternLengthColumns(uint16_t ticksPerColumn) {return ( (_patterns[_currentPattern].patternLengthTicks + 1) / RESOLUTION16TH); }
void     Track::setPatternLengthColumns(uint16_t length, uint16_t ticksPerColumn) { _patterns[_currentPattern].patternLengthTicks = length * RESOLUTION16TH - 1; }

void     Track::setPatternSpeed(uint8_t patternId, uint8_t speed) { _patterns[patternId].patternSpeed = speed; }
uint8_t  Track::getPatternSpeed(uint8_t patternId) { return _patterns[_currentPattern].patternSpeed; }

uint16_t Track::getCurrentBeat() { return _currentTrackTick / RESOLUTION;}
uint16_t Track::getCurrentColumn(uint16_t ticksPerColumn) { return _currentTrackTick / ticksPerColumn; }
uint16_t Track::getTrackDefaultNoteLengthTicks() { return _defaultNoteLengthTicks; }
void Track::setTrackDefaultNoteLengthTicks(uint16_t noteLength) { _defaultNoteLengthTicks = noteLength; }
uint8_t Track::getTrackDefaultVelocity() { return _defaultVelocity; }

void Track::_updateCuedEventIndex()
{
  if (_cuedEventIndex < _nrEvents - 1) _cuedEventIndex = _cuedEventIndex +1;
}

void Track::addEvent(uint16_t tick, uint8_t noteValue, uint8_t noteVelocity, uint8_t noteLength, bool audit)
{
  
  if (_nextFreeEventId < _nrEvents)
  {
    _patterns[_currentPattern].trackEvents[_nextFreeEventId].header = EVENT_MIDI_SLEEPING;
    _patterns[_currentPattern].trackEvents[_nextFreeEventId].tick = tick;
    _patterns[_currentPattern].trackEvents[_nextFreeEventId].noteValue = noteValue;
    _patterns[_currentPattern].trackEvents[_nextFreeEventId].noteVelocity = noteVelocity;
    _patterns[_currentPattern].trackEvents[_nextFreeEventId].noteLength = noteLength;
  }
  if(audit) _auditEvent(_patterns[_currentPattern].trackEvents[_nextFreeEventId]);
  if (tick < _currentTrackTick) _updateCuedEventIndex();
  _compactEventArray();
  _patterns[_currentPattern].patternStatus = PATTERN_FILLED;
  //printEventArray(16);

}

void Track::_auditEvent(trackEvent auditEvent)
{
  // shall ONLY be called if sequencer is not running!
  if(_noteOn_cb && (auditEvent.noteValue + transpose < 128)) _noteOn_cb(_midiChannel, auditEvent.noteValue + transpose, auditEvent.noteVelocity);
  delay(50);
  if(_noteOff_cb) _noteOff_cb(_midiChannel, auditEvent.noteValue + transpose, auditEvent.noteVelocity);
}

void Track::removeEvents(uint16_t tickStart, uint16_t tickEnd, uint8_t noteValue)
{
  // find events within tick interval and delete them
  for (uint8_t index = 0; index < _nrEvents; index++)
  {
    if((_patterns[_currentPattern].trackEvents[index].tick >= tickStart) && (_patterns[_currentPattern].trackEvents[index].tick <= tickEnd) && (_patterns[_currentPattern].trackEvents[index].noteValue == noteValue))
    {
      if(_patterns[_currentPattern].trackEvents[index].tick < _currentTrackTick)
      {
        // removed event before current time, reduce cued index ??
        _cuedEventIndex = _cuedEventIndex - 1;
      }
      _patterns[_currentPattern].trackEvents[index] = empty_trackEvent;
    }
  }

  _compactEventArray();
  //printEventArray(16);
}

void Track::clearPattern(uint8_t patternId)
{
  for (uint8_t index = 0; index < _nrEvents; index++)
  {
    _patterns[_currentPattern].trackEvents[index] = empty_trackEvent;
  }
}

void Track::clearTrack()
{
  for (int patternId = 0; patternId < NR_PATTERNS; patternId++)
   {
      clearPattern(patternId);
   }
}

uint16_t Track::getEventsInTickInterval(uint16_t tickStart, uint16_t tickEnd, uint8_t noteValue)
{
  uint16_t matchingEventCounter = 0;
  for (uint8_t index = 0; index < _nrEvents; index++)
  {
    if((_patterns[_currentPattern].trackEvents[index].tick >= tickStart) && (_patterns[_currentPattern].trackEvents[index].tick <= tickEnd) && (_patterns[_currentPattern].trackEvents[index].noteValue == noteValue) && (_patterns[_currentPattern].trackEvents[index].header != NO_EVENT))
    {
      matchingEventCounter = matchingEventCounter + 1;
    }
  }
  return matchingEventCounter;
}

int16_t Track::getEventId(uint16_t tick, uint8_t note)
{
  uint8_t index = 0;
  while (index < _nrEvents)
  {
    if (  (_patterns[_currentPattern].trackEvents[index].tick == tick)
    &&    (_patterns[_currentPattern].trackEvents[index].noteValue == note)
    &&    (_patterns[_currentPattern].trackEvents[index].header != NO_EVENT) ) return index;
    index++;
  }
  //printEventArray(16);
  return -1;
}

uint16_t Track::getEventsInTickNoteInterval(uint16_t tickStart, uint16_t tickEnd, uint8_t noteStart, uint8_t noteEnd)
{
  uint16_t matchingEventCounter = 0;
  for (uint8_t index = 0; index < _nrEvents; index++)
  {
    if((_patterns[_currentPattern].trackEvents[index].tick >= tickStart) && (_patterns[_currentPattern].trackEvents[index].tick <= tickEnd) && (_patterns[_currentPattern].trackEvents[index].noteValue >= noteStart) && (_patterns[_currentPattern].trackEvents[index].noteValue <= noteEnd) && (_patterns[_currentPattern].trackEvents[index].header != NO_EVENT))
    {
      matchingEventCounter = matchingEventCounter + 1;
    }
  }
  return matchingEventCounter;
}

void Track::_compactEventArray()
{
  qsort(_patterns[_currentPattern].trackEvents, _nrEvents, sizeof(trackEvent), compareTwoEvents);
  int index = 0;
  while(_patterns[_currentPattern].trackEvents[index].header != NO_EVENT && index < _nrEvents)
  {
    index = index +1;
  }
  _nextFreeEventId = index;
}

void Track::setHandleNoteOn(MIDIcallback cb) { _noteOn_cb = cb; }
void Track::setHandleNoteOff(MIDIcallback cb) { _noteOff_cb = cb; }

TrackEvent Track::getEvent(uint8_t patternId, uint8_t eventId) { return _patterns[patternId].trackEvents[eventId]; }

void Track::setEvent(uint8_t patternId, uint8_t eventId, TrackEvent event) { _patterns[patternId].trackEvents[eventId] = event; }

Pattern Track::getPattern(uint8_t patternId) { return _patterns[patternId];}

void Track::setPattern(uint8_t patternId, Pattern inputPattern) { _patterns[patternId] = inputPattern; }

void Track::setPatternId(uint8_t patternId)
{
  _currentPattern = patternId;
  _patterns[_currentPattern].patternStatus = PATTERN_ACTIVE;
}

void Track::cuePatternId(uint8_t patternId)
{
  _cuedPattern = patternId;
  _patterns[_cuedPattern].patternStatus = PATTERN_CUED;
}

uint8_t Track::getActivePatternId()
{
  return _currentPattern;
}

uint8_t Track::getPatternStatus(uint8_t patternId)
{
  // if empty return empty
  // if active return active
  // return _patterns[_currentPattern].patternStatus;
  
  //if (_patterns[_currentPattern].patternStatus == PATTERN_EMPTY) return PATTERN_EMPTY;
  if (_cuedPattern == patternId) return PATTERN_CUED;
  if (_currentPattern == patternId) return PATTERN_ACTIVE;
  else return PATTERN_FILLED;
}

uint8_t Track::getTrackChannel() { return _midiChannel; }
void Track::setTrackChannel(uint8_t channel) { _midiChannel =  channel; }

// debug array printouts
void Track::printEventArray(uint8_t lastIndex)
{
  Serial.print("Track: ");
  Serial.println(_trackId);

  Serial.print("_nrEvents: ");
  Serial.println(_nrEvents);
  
  Serial.print("_cuedEventIndex: ");
  Serial.println(_cuedEventIndex);

  Serial.print("Headers: ");
  for (uint8_t i = 0; i < lastIndex; i++)
  {
    Serial.print(_patterns[_currentPattern].trackEvents[i].header);
    Serial.print(", ");
  }
  Serial.println("END");
  
  Serial.print("Ticks: ");
  for (uint8_t i = 0; i < lastIndex; i++)
  {
    Serial.print(_patterns[_currentPattern].trackEvents[i].tick);
    Serial.print(", ");
  }
  Serial.println("END");

  Serial.print("Notes: ");
  for (uint8_t i = 0; i < lastIndex; i++)
  {
    Serial.print(_patterns[_currentPattern].trackEvents[i].noteValue);
    Serial.print(", ");
  }
  Serial.println("END");

  Serial.print("Lengths: ");
  for (uint8_t i = 0; i < lastIndex; i++)
  {
    Serial.print(_patterns[_currentPattern].trackEvents[i].noteLength);
    Serial.print(", ");
  }
  Serial.println("END");
}

void Track::printPlayedArray()
{
  Serial.print("Track: ");
  Serial.println(_trackId);
  Serial.print("Headers: ");
  for (uint8_t i = 0; i < NR_PLAYED_EVENTS; i++)
  {
    Serial.print(_playedEvents[i].header);
    Serial.print(", ");
  }
  Serial.println("END");

  Serial.print("Ticks: ");
  for (uint8_t i = 0; i < NR_PLAYED_EVENTS; i++)
  {
    Serial.print(_playedEvents[i].tick);
    Serial.print(", ");
  }
  Serial.println("END");
  Serial.print("Notes: ");
  for (uint8_t i = 0; i < NR_PLAYED_EVENTS; i++)
  {
    Serial.print(_playedEvents[i].noteValue);
    Serial.print(", ");
  }
  Serial.println("END");

  Serial.print("Lengths: ");
  for (uint8_t i = 0; i < NR_PLAYED_EVENTS; i++)
  {
    Serial.print(_playedEvents[i].noteLength);
    Serial.print(", ");
  }
  Serial.println("END");
}

// compare functions for qsort
int compareTwoEvents(const void *s1, const void *s2)
{
    struct trackEvent *e1 = (struct trackEvent *)s1;
    struct trackEvent *e2 = (struct trackEvent *)s2;
    // Active events first in array
    if(e1->header == NO_EVENT && e2->header != NO_EVENT) return 1;
    if(e1->header != NO_EVENT && e2->header == NO_EVENT) return -1;
    // Then sort by tick
    if(e1->tick < e2->tick) return -1;
    if(e1->tick > e2->tick) return 1;
    return 0;
}
