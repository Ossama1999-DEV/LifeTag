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

void setup() {
    Serial.begin(115200);
    delay(1000);

    preferences.begin("formulaire", false);
    loadAllData();

    Wire.begin(NFC_SDA, NFC_SCL);
    pinMode(NFC_FD, INPUT);

    WiFi.softAP(ssid, password);
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    MDNS.begin("safyra");

    setupRoutes();
    server.begin();
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();

    if (isNfcReaderPresent()) {
        Serial.println("Lecteur NFC detecte");
    }
}