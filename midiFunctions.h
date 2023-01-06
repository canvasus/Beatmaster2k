#pragma once

#include <MIDI.h>
#include <USBHost_t36.h>
#include "launchPad.h"
#include "track.h"
#include "ui.h"
#include "sequencer.h"

extern LaunchPad LP1;

void setupMidi();
void locateUsbComponents();
void getUsbDeviceName(uint8_t usbIndex, char * buf, uint8_t maxBufferSize);
void configureLaunchPad(uint8_t driverIndex);
void configureMidiInputDevices(uint8_t LP_index);
void updateMidi();
void deviceNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void deviceNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void myControlChange(uint8_t channel, uint8_t control, uint8_t value);

void voiceNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void voiceNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void serialMidiNoteOn(uint8_t channel, uint8_t noteValue, uint8_t velocity);
void serialMidiNoteOff(uint8_t channel, uint8_t noteValue, uint8_t velocity);
void midi1NoteOn(uint8_t channel, uint8_t noteValue, uint8_t velocity);
void midi1NoteOff(uint8_t channel, uint8_t noteValue, uint8_t velocity);
void midi2NoteOn(uint8_t channel, uint8_t noteValue, uint8_t velocity);
void midi2NoteOff(uint8_t channel, uint8_t noteValue, uint8_t velocity);
void midi3NoteOn(uint8_t channel, uint8_t noteValue, uint8_t velocity);
void midi3NoteOff(uint8_t channel, uint8_t noteValue, uint8_t velocity);
void midi4NoteOn(uint8_t channel, uint8_t noteValue, uint8_t velocity);
void midi4NoteOff(uint8_t channel, uint8_t noteValue, uint8_t velocity);
void midi5NoteOn(uint8_t channel, uint8_t noteValue, uint8_t velocity);
void midi5NoteOff(uint8_t channel, uint8_t noteValue, uint8_t velocity);

void LPNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void LPNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void LPControlChange(byte channel, byte control, byte value);

void transposeMidiIn(uint8_t channel, uint8_t note, uint8_t velocity);

void sendMidiClock();
void sendMidiStart();
void sendMidiStop();

#define CC_MODWHEEL 1

// -- MIDI CC from LaunchPad --

// Right side pads (19 lowest)
#define   CCtrack7        89
#define   CCtrack6        79
#define   CCtrack5        69
#define   CCtrack4        59
#define   CCtrack3        49
#define   CCtrack2        39
#define   CCtrack1        29
#define   CCstartStop     19

// Top pads (91 is rightmost)
#define   CCscrollUp       91
#define   CCscrollDown     92
#define   CCpageDecrease   93
#define   CCpageIncrease   94
#define   CCsongMode       95
#define   CCsceneMode      96
#define   CCpatternMode    97
#define   CCeditMode       98

#define   CCindicator      99

// Pads for pattern mode
#define   notePatternClear  11
#define   notePatternCopy   12
