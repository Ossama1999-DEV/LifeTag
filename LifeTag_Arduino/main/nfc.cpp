#include <Wire.h>
#include "nfc.h"
#include "config.h"
#include "app_data.h"

#define NT3H_ADDR NT3H_I2C_ADDR

static bool i2cReady = false;

static void ensureI2C() {
    if (!i2cReady) {
        Wire.begin(NFC_SDA, NFC_SCL);
        Wire.setClock(100000);   // EXACTEMENT comme le code qui a marche
        delay(100);
        i2cReady = true;
    }
}

bool nfcReadBlock(uint8_t block, uint8_t *buf) {
    ensureI2C();

    Wire.beginTransmission(NT3H_ADDR);
    Wire.write(block);

    uint8_t err = Wire.endTransmission(false);
    Serial.print("NFC read block 0x");
    if (block < 16) Serial.print("0");
    Serial.print(block, HEX);
    Serial.print(" endTransmission(false) = ");
    Serial.println(err);

    if (err != 0) return false;

    uint8_t n = Wire.requestFrom((uint8_t)NT3H_ADDR, (uint8_t)16);
    Serial.print("NFC read block 0x");
    if (block < 16) Serial.print("0");
    Serial.print(block, HEX);
    Serial.print(" requestFrom = ");
    Serial.println(n);

    if (n != 16) return false;

    for (int i = 0; i < 16; i++) {
        buf[i] = Wire.read();
    }

    return true;
}

bool nfcWriteBlock(uint8_t block, const uint8_t *data) {
    ensureI2C();

    Wire.beginTransmission(NT3H_ADDR);
    Wire.write(block);

    for (int i = 0; i < 16; i++) {
        Wire.write(data[i]);
    }

    uint8_t err = Wire.endTransmission(true);

    Serial.print("NFC write block 0x");
    if (block < 16) Serial.print("0");
    Serial.print(block, HEX);
    Serial.print(" err = ");
    Serial.println(err);

    delay(10);   // EXACTEMENT comme le code qui a marche

    return (err == 0);
}

static bool writeNdefText(const String& text) {
    const uint8_t langLen = 2; // "fr"

    if (text.length() > 250) {
        Serial.println("NFC: texte trop long");
        return false;
    }

    uint8_t payloadLen = 1 + langLen + text.length();

    //uint8_t buffer[64] = {0};
    uint8_t buffer[256] = {0};
    int p = 0;

    // TLV NDEF
    buffer[p++] = 0x03;

    // longueur du record NDEF
    uint8_t ndefLen = 4 + payloadLen;
    buffer[p++] = ndefLen;

    // NDEF Text Record
    buffer[p++] = 0xD1;
    buffer[p++] = 0x01;
    buffer[p++] = payloadLen;
    buffer[p++] = 0x54; // 'T'

    // payload
    buffer[p++] = langLen;
    buffer[p++] = 'f';
    buffer[p++] = 'r';

    for (size_t i = 0; i < text.length(); i++) {
        buffer[p++] = (uint8_t)text[i];
    }

    buffer[p++] = 0xFE;

    Serial.print("NFC: taille totale buffer = ");
    Serial.println(p);

    uint8_t totalBlocks = (p + 15) / 16;

    for (uint8_t i = 0; i < totalBlocks; i++) {
        uint8_t temp[16] = {0};

        for (int j = 0; j < 16; j++) {
            int index = i * 16 + j;
            if (index < p) {
                temp[j] = buffer[index];
            }
        }

        uint8_t block = 0x01 + i;

        Serial.print("NFC: ecriture bloc ");
        Serial.println(block, HEX);

        if (!nfcWriteBlock(block, temp)) {
            Serial.print("NFC: echec ecriture bloc ");
            Serial.println(block, HEX);
            return false;
        }
    }

    return true;
}

/*
bool writeEmergencyToNFC() {
    Serial.println("NFC: debut writeEmergencyToNFC");

    String text = prenom + " " + nom;

    if (groupeSanguin.length() > 0) {
        text += "\nGS: " + groupeSanguin;
    }

    if (allergies.length() > 0) {
        text += "\nAllergies: " + allergies;
    }

    if (traitementsVitaux.length() > 0) {
        text += "\nVitaux: " + traitementsVitaux;
    }

    if (contactUrgence.length() > 0) {
        text += "\nUrgence: " + contactUrgence;
    }

    Serial.println("NFC: texte a ecrire =");
    Serial.println(text);
    Serial.print("NFC: longueur texte = ");
    Serial.println(text.length());

    bool ok = writeNdefText(text);

    Serial.print("NFC: resultat final = ");
    Serial.println(ok ? "OK" : "ECHEC");

    return ok;
}*/

String clean (String input) {
    input.replace(";", ",");   // évite de casser le format
    input.replace("\n", " ");  // évite les retours à la ligne
    input.trim();              // nettoie les espaces inutiles
    return input;
}

bool writeEmergencyToNFC() {
    Serial.println("NFC: debut writeEmergencyToNFC");
/*
    String text =
        "\"" + nom + "\";" +
        "\"" + prenom + "\";" +
        "\"" + dateNaissance + "\";" +
        "\"" + sexe + "\";" +
        "\"" + adresse + "\";" +
        "\"" + contactUrgence + "\";" +
        "\"" + taille + "\";" +
        "\"" + poids + "\";" +
        "\"" + groupeSanguin + "\";" +
        "\"" + allergies + "\";" +
        "\"" + maladies + "\";" +
        "\"" + traitement + "\";" +
        "\"" + implant + "\"";
        */

    String text =
        clean(nom) + ";" +
        clean(prenom) + ";" +
        clean(dateNaissance) + ";" +
        clean(sexe) +  ";" +
        clean(adresse) + ";" +
        clean(contactUrgence) + ";" +
        clean(taille) + ";" +
        clean(poids) + ";" +
        clean(groupeSanguin) + ";" +
        clean(allergies) + ";" +
        clean(maladies) + ";" +
        clean(traitement) + ";" +
        clean(dispositifMedical) ;

    Serial.println("NFC: texte a ecrire =");
    Serial.println(text);
    Serial.print("NFC: longueur texte = ");
    Serial.println(text.length());

    bool ok = writeNdefText(text);

    Serial.print("NFC: resultat final = ");
    Serial.println(ok ? "OK" : "ECHEC");

    return ok;
}