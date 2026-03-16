#include "nfc.h"
#include "config.h"
#include "app_data.h"
#include <Wire.h>

bool isNfcReaderPresent() {
    return digitalRead(NFC_FD) == HIGH;
}

void writeNfcPage(uint8_t page, uint8_t *data) {
    Wire.beginTransmission(NT3H_I2C_ADDR);
    Wire.write(page);
    for (int i = 0; i < 16; i++) {
        Wire.write(data[i]);
    }
    Wire.endTransmission();
}

void writeNfcText(String text) {
    uint8_t buffer[16];
    int page = 4;
    int len = text.length();
    int index = 0;

    while (index < len) {
        for (int i = 0; i < 16; i++) {
            if (index < len) {
                buffer[i] = text[index++];
            } else {
                buffer[i] = 0x00;
            }
        }
        writeNfcPage(page, buffer);
        page++;
    }
}

void writeEmergencyToNFC() {
    String data = "";
    data += "Nom:" + nom + "\n";
    data += "Prenom:" + prenom + "\n";
    data += "Groupe:" + groupeSanguin + "\n";
    data += "Allergies:" + allergies + "\n";
    data += "Contact:" + contactUrgence + "\n";

    writeNfcText(data);
}