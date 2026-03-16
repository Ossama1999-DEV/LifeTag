#pragma once

#include <Arduino.h>

#define NFC_SDA 8
#define NFC_SCL 9
#define NFC_FD 10

#define NT3H_I2C_ADDR 0x55

extern const char* adminUser;
extern const char* adminPass;

extern const char* normalUser;
extern const char* normalPass;

extern const char* ssid;
extern const char* password;