#include "sdMgr.h"

const int chipSelect = BUILTIN_SDCARD;


 
const char NAstring[11] = {'N', '/', 'A', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
fileInfo FileInfo;

void initSDcard()
{
  if (!SD.begin(chipSelect)) { Serial.println("Card failed, or not present"); }
}

uint8_t peekFileName(uint8_t fileNr, char * buf)
{
  char fileName[18];
  sprintf(fileName, "BM2K_DATAFILE_%03d", fileNr);
  Serial.println(fileName);
  if (SD.exists(fileName))
  {
    File dataFile = SD.open(fileName, FILE_READ);
    dataFile.read((uint8_t *)&FileInfo, sizeof(FileInfo));
    dataFile.close();
    Serial.print("Patch nr: ");
    Serial.print(fileNr);
    Serial.print(" contains name: ");
    Serial.println(FileInfo.name);
    memcpy(buf, FileInfo.name, 11);
    return 1;
  }
  else
  {
    Serial.printf("Patch nr %d not found on SD", fileNr);
    memcpy(buf, NAstring, 11);
    return 0;
  }
}

void loadFile(uint8_t fileNr)
{
  Serial.print("Loading patch nr: ");
  Serial.println(fileNr);
  char fileName[18];
  sprintf(fileName, "BM2K_DATAFILE_%03d", fileNr);
  if (SD.exists(fileName))
  {
    File dataFile = SD.open(fileName, FILE_READ);
    dataFile.read((uint8_t *)&FileInfo, sizeof(FileInfo));
    if (FileInfo.dataSize == DATA_SIZE)
    {
      for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
      {
        for (uint8_t patternId = 0; patternId < NR_PATTERNS; patternId++)
        {
          Pattern tempPattern;
          dataFile.read((uint8_t *)&tempPattern, sizeof(tempPattern));
          tracks[trackId]->setPattern(patternId, tempPattern);  
        }
      }
      dataFile.read((uint8_t *)&SequencerData, sizeof(SequencerData));
    }
    else Serial.println("Invalid data size on SD, not loaded");
    dataFile.close();
  }
  else Serial.println("File does not exist on SD");
}

void saveFile(uint8_t fileNr)
{
  Serial.print("Storing patch nr: ");
  Serial.println(fileNr);
  char fileName[18];
  sprintf(fileName, "BM2K_DATAFILE_%03d", fileNr);
  SD.remove(fileName);
  File dataFile = SD.open(fileName, FILE_WRITE);
  FileInfo.dataSize = DATA_SIZE;
  dataFile.write((uint8_t *)&FileInfo, sizeof(FileInfo));
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
  {
    for (uint8_t patternId = 0; patternId < NR_PATTERNS; patternId++)
    {
      Pattern tempPattern = tracks[trackId]->getPattern(patternId);
      dataFile.write((uint8_t *)&tempPattern, sizeof(tempPattern));
    }
  }
  dataFile.write((uint8_t *)&SequencerData, sizeof(SequencerData));
  dataFile.close();
}
