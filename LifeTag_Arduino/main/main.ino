#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  5    // SDA/NSS
#define RST_PIN 27

MFRC522 mfrc522(SS_PIN, RST_PIN); 

byte block = 4; // Bloc où on écrit
MFRC522::MIFARE_Key key;

// === Définition des broches ===
const int ledPinRed = 26;
const int ledPinGreen = 25;


// --- Fonction écriture dans un bloc ---
bool writeBlock(byte blockNumber, byte *dataPointer) {
  MFRC522::StatusCode status;

  status = mfrc522.PCD_Authenticate(
    MFRC522::PICC_CMD_MF_AUTH_KEY_A, 
    blockNumber, &key, &(mfrc522.uid)
  );
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Auth erreur : ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }

  status = mfrc522.MIFARE_Write(blockNumber, dataPointer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Écriture erreur : ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }
  return true;
}

// --- Fonction lecture d’un bloc ---
bool readBlock(byte blockNumber, byte *dataBuffer) {
  MFRC522::StatusCode status;
  byte bufferSize = 18;

  status = mfrc522.PCD_Authenticate(
    MFRC522::PICC_CMD_MF_AUTH_KEY_A, 
    blockNumber, &key, &(mfrc522.uid)
  );
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Auth erreur : ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }

  status = mfrc522.MIFARE_Read(blockNumber, dataBuffer, &bufferSize);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Lecture erreur : ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }
  return true;
}


void setup() {
  Serial.begin(115200);

  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);

#if defined(ARDUINO_ARCH_ESP32)
  SPI.begin(18, 19, 23, SS_PIN);
#else
  SPI.begin();  // Pour Arduino Uno (Proteus)
#endif

  mfrc522.PCD_Init();
  Serial.println("Approche un badge RFID...");

  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; // Clé par défaut

  digitalWrite(ledPinRed, HIGH);
  digitalWrite(ledPinGreen, LOW);
}

void loop() {
  // digitalWrite(ledPinGreen, HIGH);
  // digitalWrite(ledPinRed, HIGH);

  // Attente carte
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.println("Carte détectée !");

  // 🔹 Écriture du message
  if (writeBlock(block, (byte*)"HELLO FROM ESP32!!")) {
    Serial.println("Écriture OK");
    digitalWrite(ledPinGreen, HIGH);
  } else {
    Serial.println("Erreur écriture !");
    digitalWrite(ledPinRed, HIGH);
    delay(2000);
    return;
  }

  delay(200);

  // 🔹 Lecture du message
  byte buffer[18];
  if (readBlock(block, buffer)) {
    Serial.print("Lecture OK : ");
    for (int i = 0; i < 16; i++) Serial.print((char)buffer[i]);
    Serial.println();
    digitalWrite(ledPinGreen, HIGH);
  } else {
    Serial.println("Erreur lecture !");
    digitalWrite(ledPinRed, HIGH);
  }

  delay(2000);
  digitalWrite(ledPinGreen, LOW);
  digitalWrite(ledPinRed, LOW);

  // Libérer carte
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}