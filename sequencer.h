#pragma once

#include <Arduino.h>
#include "track.h"
#include "midiFunctions.h"

#define STATE_STOPPED 0
#define STATE_RUNNING 1

#define PLAYMODE_NONE   0
#define PLAYMODE_CHAIN  1

#define MODE_PATTERNEDIT 0
#define MODE_EVENTEDIT  1

#define SELECTION_PATTERN 0
#define SELECTION_COLUMNS 1
#define SELECTION_VIEW    2
#define SELECTION_

#define ACTION_CLEAR      0
#define ACTION_COPY       1
#define ACTION_PASTE      2

#define NR_TRACKS 7
#define NR_PATTERNS_IN_ARRANGEMENT 8
#define NO_SCENE 255

#define NR_SCENES 16

#define NR_ARRPOSITIONS 56       // Max scenes in arrangement

#define ticksPerBeat 24         // 1/4 * 1/24 = 1/96th resolution
#define TICKS_PER_BEAT    24
#define TICKS_PER_COLUMN  6


struct sequencerData
{
  uint8_t bpm = 120;
  uint8_t scenes[NR_TRACKS][NR_SCENES];
  uint8_t sceneColors[NR_SCENES];
  uint8_t arrangement[NR_ARRPOSITIONS];
};

extern sequencerData SequencerData;

extern uint8_t sequencerState;
extern uint8_t sequencerEditMode;
extern String outputNames[];
extern Track *tracks[NR_TRACKS];
extern uint8_t currentTrack;
extern uint8_t currentPattern;
extern int16_t currentEvent;
extern uint8_t currentScene;

void initSequencer();
void tickTracks();

void startSequencer();
void stopSequencer();

void setBpm(float bpm);
float getBpm();

float getPlaymode();
void setPlaymode(float mode);

void setMidiClockOnOff(float onOff);
float getMidiClockOnOff();

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

void updateSceneConfiguration(uint8_t sceneId);
void setScene(uint8_t sceneId);
float getSceneNr();
void setSceneNr(float sceneId);
float getSceneColor();
void setSceneColor(float color);

void setToolSelection(float selection);
float getToolSelection();
String getSelectionEnum(uint8_t selection);

void setToolAction(float action);
float getToolAction();
String getActionEnum(uint8_t action);

void setToolTranspose(float selection);
float getToolTranspose();

void doToolAction();
void copySelection();
void pasteSelection();
void clearSelection();
void clearEventClipBoard();

void updateSequencer();
void updateSongMode();
