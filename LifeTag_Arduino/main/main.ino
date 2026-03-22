
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Adafruit_GFX.h>
#include <Preferences.h>
#include <ESPmDNS.h>
#include <Wire.h>

#include "config.h"
#include "app_data.h"
#include "storage.h"
#include "web_handlers.h"
#include "nfc.h"

void testNfcWriteAtBoot() {
    uint8_t block1[16] = {
        0x03, 0x0D,
        0xD1, 0x01, 0x09,
        0x55,
        0x04,
        't','a','s','t','.','f','r',
        0xFE, 0x00
    };

    uint8_t block2[16] = {0};
    uint8_t block3[16] = {0};

    Serial.println("TEST NFC BOOT");

    bool ok1 = nfcWriteBlock(0x01, block1);
    bool ok2 = nfcWriteBlock(0x02, block2);
    bool ok3 = nfcWriteBlock(0x03, block3);

    Serial.print("BOOT NFC result = ");
    Serial.println((ok1 && ok2 && ok3) ? "OK" : "ECHEC");
}


void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("BOOT");

    Wire.begin(NFC_SDA, NFC_SCL);
    Wire.setClock(100000);
    delay(100);

    Serial.println("NFC initialise");
    //testNfcWriteAtBoot();

    preferences.begin("formulaire", false);
    loadAllData();

    pinMode(NFC_FD, INPUT);

    WiFi.softAP(ssid, password);
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

    if (MDNS.begin("safyra")) {
        Serial.println("MDNS OK");
    } else {
        Serial.println("MDNS FAIL");
    }

    setupRoutes();
    server.begin();
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();

    if (nfcWritePending && millis() - nfcWriteRequestTime > 2000) {
        nfcWritePending = false;

        Serial.println("NFC: lancement ecriture differee");

        bool ok = writeEmergencyToNFC();

        Serial.print("NFC: ecriture differee = ");
        Serial.println(ok ? "OK" : "ECHEC");
    }
}
/*
void scanI2C() {
  Serial.println("Scan I2C...");
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();
    if (err == 0) {
      Serial.print("Peripherique trouve a 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
    }
  }
  Serial.println("Fin scan");
}

#include <Wire.h>

#define SDA_PIN 8
#define SCL_PIN 9

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  delay(200);

  Serial.println("Scan I2C...");
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();

    if (err == 0) {
      Serial.print("Peripherique trouve a 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
    }
    delay(2);
  }
  Serial.println("Fin scan");
}

void loop() {}*/