// === Définition des broches ===
const int ledPinRed = 21;
const int ledPinYellow = 18;
const int ledPinGreen = 5;
const int buttonPin = 19;

// === Variables ===
bool buttonPressed = false;

void setup() {
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinYellow, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);  // bouton connecté à GND => appui = LOW

  // Démarre avec le feu vert
  digitalWrite(ledPinGreen, HIGH);
}

void loop() {
  // Lecture du bouton
  if (digitalRead(buttonPin) == LOW) {  
    buttonPressed = true;
  }

  // Si le bouton est pressé, on exécute un cycle piéton
  if (buttonPressed) {
    // Éteindre le vert et allumer le jaune (transition)
    digitalWrite(ledPinGreen, LOW);
    digitalWrite(ledPinYellow, HIGH);
    delay(1000);

    // Éteindre jaune et allumer rouge (arrêt voitures)
    digitalWrite(ledPinYellow, LOW);
    digitalWrite(ledPinRed, HIGH);
    delay(3000);  // feu rouge pendant 3 sec (piétons traversent)

    // Transition rouge → vert
    digitalWrite(ledPinYellow, HIGH);
    delay(1000);

    // Revenir à vert
    digitalWrite(ledPinRed, LOW);
    digitalWrite(ledPinYellow, LOW);
    digitalWrite(ledPinGreen, HIGH);

    // Reset du bouton
    buttonPressed = false;
  }

  // Sinon, on reste au vert (feu normal)
  delay(100);
}
