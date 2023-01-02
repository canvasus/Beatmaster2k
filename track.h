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

#define MUTE_OFF            0
#define MUTE_ON             1
#define MUTE_ON_CUED        2
#define MUTE_OFF_CUED       3

#define NR_TRACK_EVENTS     128
#define NR_PLAYED_EVENTS    16
#define RESOLUTION          24 // ticks per beat (1/4th)
#define RESOLUTION16TH      6  // ticks per 1/16th --> ticks per index

#define NR_TRACKS           7
#define NR_PATTERNS         8

#define MIDICH_NOTEVALUE 17

#define PATTERN_SPEED_x1_4   0
#define PATTERN_SPEED_x1_3   1
#define PATTERN_SPEED_x1_2   2
#define PATTERN_SPEED_x1     3
#define PATTERN_SPEED_x2     4

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
  uint8_t       patternSpeed;
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
    uint8_t       _prescalerCounter;

    uint8_t       _tickFlag;
    bool          _cuedMuteFlag;
    bool          _cuedMuteStatus;
    
    // Track configuration
    
    uint8_t       _midiChannel;
    uint8_t       _defaultNoteLengthTicks;
    uint8_t       _defaultVelocity;
    
    MIDIcallback  _noteOn_cb;
    MIDIcallback  _noteOff_cb;
        
    void          _compactEventArray();
    void          _updateCuedEventIndex();
    void          _triggerEvents();
    void          _handleAutoNoteOff();
    void          _copyToPlayedBuffer(trackEvent event);
    void          _auditEvent(trackEvent event);
    uint16_t      _tickPrescaler();
    
      
  public:
    Track(uint8_t trackId, String trackName, uint8_t trackColor);
    String        name;
    uint8_t       outputId;
    uint8_t       color;
    int           transpose;
    uint8_t       trackRespondToTranspose;
    bool          trackMuted;
    uint8_t       lowerRow;

    //void          update();
   
    uint8_t       getId();
    uint16_t      getEventTick(uint8_t eventId);
    uint16_t      getCurrentTrackTick();
    
    void          setPatternLengthBeats(uint16_t lengthBeats);
    uint16_t      getPatternLengthBeats();
    uint16_t      getPatternLengthColumns(uint16_t ticksPerColumn);
    uint16_t      getPatternLengthColumns_indexed(uint8_t patternId, uint16_t ticksPerColumn);
    void          setPatternLengthColumns(uint16_t length, uint16_t ticksPerColumn);
    uint16_t      getTrackDefaultNoteLengthTicks();
    void          setTrackDefaultNoteLengthTicks(uint16_t noteLength);
    uint8_t       getTrackDefaultVelocity();
    uint16_t      getCurrentBeat();
    uint16_t      getCurrentColumn(uint16_t ticksPerColumn);
    
    void          addEvent(uint16_t tick, uint8_t noteValue, uint8_t noteVelocity, uint8_t noteLength, bool audit);
    void          removeEvents(uint16_t tickStart, uint16_t tickEnd, uint8_t noteValue);
    uint16_t      getEventsInTickInterval(uint16_t tickStart, uint16_t tickEnd, uint8_t noteValue);
    uint16_t      getEventsInTickNoteInterval(uint16_t tickStart, uint16_t tickEnd, uint8_t noteStart, uint8_t noteEnd);
    int16_t       getEventId(uint16_t tick, uint8_t note);
    uint8_t       getPatternStatus(uint8_t patternId);
  
    void          clearPattern(uint8_t patternId);
    void          clearTrack();

    void          setPatternId(uint8_t patternId);
    void          cuePatternId(uint8_t patternId);
    uint8_t       getActivePatternId();

    void          cueMuteStatus(bool status);
    uint8_t       getMuteStatus();

    TrackEvent    getEvent(uint8_t patternId, uint8_t eventId);
    void          setEvent(uint8_t patternId, uint8_t eventId, TrackEvent event);

    Pattern       getPattern(uint8_t patternId);
    void          setPattern(uint8_t patternId, Pattern inputPattern);

    void          setPatternSpeed(uint8_t patternId, uint8_t speed);
    uint8_t       getPatternSpeed(uint8_t patternId);

    void          tickTrack(); // sent from main sequencer at defined tick rate 1/96th
    
    void          loopResetTrack();
    void          flushPlayedBuffer();
    void          printEventArray(uint8_t lastIndex);
    void          printPlayedArray();
   
    void          setHandleNoteOn(MIDIcallback cb);
    void          setHandleNoteOff(MIDIcallback cb);

    uint8_t getTrackChannel();
    void setTrackChannel(uint8_t channel);
   
};
