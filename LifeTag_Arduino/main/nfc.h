#pragma once

#include <Arduino.h>
#include <stdint.h>

bool isNfcReaderPresent();
void writeNfcPage(uint8_t page, uint8_t *data);
void writeNfcText(String text);
void writeEmergencyToNFC();