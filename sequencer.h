#pragma once
#include <Arduino.h>

#define STATE_STOPPED 0
#define STATE_RUNNING 1

#define MODE_PATTERNEDIT 0
#define MODE_EVENTEDIT  1

#define NR_TRACKS 7
#define NR_PATTERNS_IN_ARRANGEMENT 8
#define ARR_NO_PATTERN 254

#define ticksPerBeat 24         // 1/4 * 1/24 = 1/96th resolution

extern uint8_t sequencerState;
extern uint8_t sequencerEditMode;
extern uint16_t   bpm ;
extern uint8_t arrangement[NR_TRACKS][NR_PATTERNS_IN_ARRANGEMENT];

void initSequencer();
void updateSequencer();
void tickTracks();

void startSequencer();
void stopSequencer();

void setBpm(float bpm);
float getBpm();

void setCurrentTrack(uint8_t track);

void setTrackOutput(float value);
float getTrackOutput();
String getOutputEnum(uint8_t value);

float getTrackChannel();
void setTrackChannel(float channel);
String get0_16_note(uint8_t channel);

float getTrackPatternNr();
void setTrackPatternNr(float patternNr);

float getTrackLengthColumns();
void setTrackLengthColumns(float columns);

float getTrackDefLength();
void setTrackDefLength(float length);

float getEventLength();
void setEventLength(float length);

void setTranspose(int transpose);
float getTransposeStatus();
void setTransposeStatus(float status);
String getNoYesSelectedEnum(uint8_t value);

void setPatternSpeed(float speed);
float getPatternSpeed();
String getPatternSpeedEnum(uint8_t value);

void resetTracks();
void flushTracksPlayedBuffers();
