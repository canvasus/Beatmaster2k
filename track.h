#pragma once
#include "Arduino.h"

#define NO_EVENT            0
#define EVENT_MIDI_PLAYED   1
#define EVENT_MIDI_SLEEPING 2
#define EVENT_CC_PLAYED     3
#define EVENT_CC_SLEEPING   4
#define EVENT_MIDI_NEW      5

#define PATTERN_EMPTY       0
#define PATTERN_FILLED      1
#define PATTERN_ACTIVE      2
#define PATTERN_CUED        3

#define NR_TRACK_EVENTS     128
#define NR_PLAYED_EVENTS    16
#define RESOLUTION          24 // ticks per beat (1/4th)

#define NR_PATTERNS         8

const uint8_t scale_major[8] = {0, 2, 4, 5, 7, 9, 11, 12};
const uint8_t scale_minor[8] = {0, 2, 3, 5, 7, 8, 10, 12};

struct trackEvent{
    uint8_t       header;
    uint16_t      tick;
    uint8_t       noteValue;
    uint8_t       noteVelocity;
    uint16_t      noteLength;
};

typedef struct trackEvent TrackEvent;

const trackEvent empty_trackEvent = {0,0,0,0,0};

int compareTwoEvents(const void *s1, const void *s2);

struct pattern {
  uint8_t       patternId;
  uint16_t      patternLengthTicks;
  uint8_t       patternStatus;
  TrackEvent    trackEvents[NR_TRACK_EVENTS];
};

typedef struct pattern Pattern;

typedef void (*MIDIcallback)(uint8_t channel, uint8_t note, uint8_t velocity);

class Track {
  
  private:
    uint8_t       _trackId;
    uint16_t      _currentTrackTick;
    uint8_t       _nrEvents;
    uint8_t       _nextFreeEventId;
    Pattern       _patterns[NR_PATTERNS];
    TrackEvent    _playedEvents[NR_PLAYED_EVENTS];
    uint8_t       _cuedEventIndex;
    uint8_t       _currentPattern;
    uint8_t       _cuedPattern;

    // Track configuration
    bool          _trackRespondToTranspose;
    //bool          _trackMuted;
    uint8_t       _midiChannel;
    uint8_t       _defaultNoteLengthTicks;
    
    MIDIcallback  _noteOn_cb;
    MIDIcallback  _noteOff_cb;
        
    void          _compactEventArray();
    void          _updateCuedEventIndex();
    void          _triggerEvents();
    void          _handleAutoNoteOff();
    void          _copyToPlayedBuffer(trackEvent event);
    void          _auditEvent(trackEvent event);
    
  public:
    Track(uint8_t trackId, String trackName, uint8_t trackColor);
    String        name;
    uint8_t       outputId;
    uint8_t       color;
    int           transpose;
    uint8_t       lowerRow;
   
    uint8_t       getId();
    uint16_t      getEventTick(uint8_t eventId);
    uint16_t      getCurrentTrackTick();
    
    void          setPatternLengthBeats(uint16_t lengthBeats);
    uint16_t      getPatternLengthBeats();
    uint16_t      getPatternLengthColumns(uint16_t ticksPerColumn);
    uint16_t      getTrackDefaultNoteLengthTicks();
    void          setTrackDefaultNoteLengthTicks(uint16_t noteLength);
    uint16_t      getCurrentBeat();
    uint16_t      getCurrentColumn(uint16_t ticksPerColumn);
    
    void          addEvent(uint16_t tick, uint8_t noteValue, uint8_t noteVelocity, uint8_t noteLength, bool audit);
    void          removeEvents(uint16_t tickStart, uint16_t tickEnd, uint8_t noteValue);
    uint16_t      getEventsInTickInterval(uint16_t tickStart, uint16_t tickEnd, uint8_t noteValue);
    uint16_t      getEventsInTickNoteInterval(uint16_t tickStart, uint16_t tickEnd, uint8_t noteStart, uint8_t noteEnd);
    uint8_t       getPatternStatus(uint8_t patternId);

    void          clearPattern(uint8_t patternId);
    void          clearTrack();

    void          setPatternId(uint8_t patternId);
    void          cuePatternId(uint8_t patternId);
    uint8_t       getActivePatternId();

    TrackEvent    getEvent(uint8_t patternId, uint8_t eventId);
    void          setEvent(uint8_t patternId, uint8_t eventId, TrackEvent event);

    Pattern       getPattern(uint8_t patternId);
    void          setPattern(uint8_t patternId, Pattern inputPattern);

    void          tickTrack(); // sent from main sequencer at defined tick rate 1/96th
    void          loopResetTrack();
    void          flushPlayedBuffer();
    void          printEventArray(uint8_t lastIndex);
    void          printPlayedArray();
   
    void          setHandleNoteOn(MIDIcallback cb);
    void          setHandleNoteOff(MIDIcallback cb);



    
    
};
