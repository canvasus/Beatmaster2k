#pragma once
#include <Arduino.h>

#define STATE_STOPPED 0
#define STATE_RUNNING 1

#define NR_TRACKS 5

extern uint8_t sequencerState;
void initSequencer();
void updateSequencer();

void startSequencer();
void stopSequencer();

void setBpm(float bpm);
float getBpm();

void setTrackOutput(float value);
float getTrackOutput();
String getOutputEnum(uint8_t value);

void resetTracks();
void flushTracksPlayedBuffers();
