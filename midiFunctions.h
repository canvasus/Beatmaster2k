#pragma once

#include <MIDI.h>
#include <USBHost_t36.h>
#include "launchPad.h"
#include "track.h"
#include "voice.h"
#include "ui.h"

void setupMidi();
void locateUsbComponents();
void configureLocalKeys(uint8_t driverIndex);
void configureLaunchPad(uint8_t driverIndex);
void updateMidi();
void localKeysNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void deviceNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void localKeysNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void deviceNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void myControlChange(uint8_t channel, uint8_t control, uint8_t value);

void LPNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void LPNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);

#define CC_MODWHEEL 1

//MIDI CC from LaunchPad

// Right side pads (19 lowest)
#define   hostBpmIncrease 89
#define   hostBpmDecrease 79
#define   CCmidiTrack3    69
#define   CCmidiTrack2    59
#define   CCmidiTrack1    49
#define   CCdrumTrack2    39
#define   CCdrumTrack1    29
#define   hostStartStop   19

// Top pads (91 is rightmost)
#define   CCscrollUp       91
#define   CCscrollDown     92
#define   hostPageDecrease 93
#define   hostPageIncrease 94
#define   CCpatternSelect  95
#define   TBD96            96
#define   TBD97            97
#define   mixerMatrix      98

// Pads for pattern mode
#define   notePatternClear  11
#define   notePatternCopy   12
