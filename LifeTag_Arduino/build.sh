#!/usr/bin/env bash
set -euo pipefail

SKETCH_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_INO="$SKETCH_DIR/LifeTag_Arduino.ino"
SRC_INO="$SKETCH_DIR/main/main.ino"

mkdir -p "$SKETCH_DIR/bin/avr" "$SKETCH_DIR/bin/esp32"

# S'assurer qu'il y a un .ino à la racine du sketch
if [[ -f "$SRC_INO" ]]; then
  cp -f "$SRC_INO" "$ROOT_INO"
fi

echo "[AVR] Compilation pour Arduino Uno (Proteus)"
arduino-cli compile \
  --fqbn arduino:avr:uno \
  --output-dir "$SKETCH_DIR/bin/avr" \
  --verbose "$SKETCH_DIR"

echo "[ESP32] Compilation pour carte ESP32 (matériel réel)"
arduino-cli compile \
  --fqbn esp32:esp32:esp32 \
  --output-dir "$SKETCH_DIR/bin/esp32" \
  --verbose "$SKETCH_DIR"

echo
echo "Fichiers générés :"
echo "  Proteus (AVR) : $SKETCH_DIR/bin/avr/LifeTag_Arduino.ino.elf"
echo "  ESP32 (réel)  : $SKETCH_DIR/bin/esp32/LifeTag_Arduino.ino.elf"
