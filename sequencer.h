#pragma once
#include <Arduino.h>

#define STATE_STOPPED 0
#define STATE_RUNNING 1

#define MODE_PATTERNEDIT 0
#define MODE_EVENTEDIT  1

#define NR_TRACKS 7
#define NR_PATTERNS_IN_ARRANGEMENT 8

#define ticksPerBeat 24         // 1/4 * 1/24 = 1/96th resolution

extern uint8_t sequencerState;
extern uint8_t sequencerEditMode;
extern uint16_t   bpm ;

void initSequencer();
void updateSequencer();

void startSequencer();
void stopSequencer();

void setBpm(float bpm);
float getBpm();

void setTrackOutput(float value);
float getTrackOutput();
String getOutputEnum(uint8_t value);

float getTrackChannel();
void setTrackChannel(float channel);

float getTrackLengthColumns();
void setTrackLengthColumns(float columns);

float getEventLength();
void setEventLength(float length);

void setTranspose(int transpose);
float getTransposeStatus();
void setTransposeStatus(float status);
String getNoYesSelectedEnum(uint8_t value);

void resetTracks();
void flushTracksPlayedBuffers();
