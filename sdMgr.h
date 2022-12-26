#pragma once

#include <SD.h>
#include "midiFunctions.h"
#include "sequencer.h"

#define MAX_FILES 128

#define DATA_SIZE_OLD sizeof(pattern) * NR_TRACKS * NR_PATTERNS + sizeof(SequencerDataOld)
#define DATA_SIZE sizeof(pattern) * NR_TRACKS * NR_PATTERNS + sizeof(SequencerData)

struct fileInfo
{
  uint16_t dataSize = 0;
  char name[11] = {'I', 'N', 'I', 'T', ' ', 'P', 'A', 'T', 'C', 'H'};
};

extern fileInfo PatchInfo;

void initSDcard();
uint8_t peekFileName(uint8_t fileNr, char * buf);
void loadFile(uint8_t fileNr);
void saveFile(uint8_t fileNr);
