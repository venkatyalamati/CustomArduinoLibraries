#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include "GlobalConstants.h" // include here only to use this in other projects
void setupTimer1();
void enableTimer1();
void disableTimer1();
void setupTimer2();
void enableTimer2();
void disableTimer2();
bool erase_eeprom_if_req(int addrValStr);
void insertFloatVal_intoCharArray(char *charArr, uint8_t startPos, uint8_t fullDcmlLen, uint8_t numDcmlPts, float dcmlVal, bool dispPosSign);
void insertUintVal_intoCharArray(char *charArr, uint8_t startPos, uint8_t fullDcmlLen, uint32_t dcmlVal);
void insertIntVal_intoCharArray(char *charArr, uint8_t startPos, uint8_t fullDcmlLen, int32_t dcmlVal, bool dispPosSign);
float floatFromCharArray(const char *str);