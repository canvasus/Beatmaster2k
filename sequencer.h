#pragma once
#include <Arduino.h>

#define STATE_STOPPED 0
#define STATE_RUNNING 1

#define MODE_PATTERNEDIT 0
#define MODE_EVENTEDIT  1

#define NR_TRACKS 5

#define ticksPerBeat 24         // 1/4 * 1/24 = 1/96th resolution

extern uint8_t sequencerState;
extern uint8_t sequencerEditMode;

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

void resetTracks();
void flushTracksPlayedBuffers();
