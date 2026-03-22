#pragma once
#include <Arduino.h>

bool nfcReadBlock(uint8_t block, uint8_t *buf);
bool nfcWriteBlock(uint8_t block, const uint8_t *data);
bool writeEmergencyToNFC();