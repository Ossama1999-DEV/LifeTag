#!/usr/bin/env bash
set -euo pipefail

# ============ Réglages ============
SKETCH_ROOT=""   # sera auto-détecté
BIN_UNO="bin/uno"
BIN_ESP32="bin/esp32"
BAUD="${BAUD:-921600}"
PORT="${PORT:-}"         # override: PORT=/dev/ttyUSB0 ./build_and_flash.sh --flash esp32
TARGET="${1:-all}"       # all | uno | esp32 | --flash (alias: flash tout de suite après build)
DO_FLASH="no"

# Alias d’options simples
if [[ "$TARGET" == "--flash" ]]; then
  TARGET="all"
  DO_FLASH="yes"
fi

# ============ Helpers ============
log() { echo -e "\033[1;34m[INFO]\033[0m $*"; }
warn(){ echo -e "\033[1;33m[WARN]\033[0m $*"; }
err() { echo -e "\033[1;31m[ERR]\033[0m  $*"; exit 1; }

need() {
  command -v "$1" >/dev/null 2>&1 || err "Outil requis manquant: $1"
}

# ============ Détection sketch ============
if [[ -f "LifeTag_Arduino.ino" ]]; then
  SKETCH_ROOT="."
elif [[ -f "main/main.ino" ]]; then
  SKETCH_ROOT="main"
else
  err "Sketch introuvable. Place un 'LifeTag_Arduino.ino' à la racine OU 'main/main.ino'."
fi
log "Sketch détecté dans: $SKETCH_ROOT"

# ============ Pré-requis ============
need arduino-cli
mkdir -p "$BIN_UNO" "$BIN_ESP32"

# S’assure que les cores sont installés
if ! arduino-cli core list | grep -q '^arduino:avr'; then
  log "Installation du core arduino:avr…"
  arduino-cli core update-index
  arduino-cli core install arduino:avr
fi

if ! arduino-cli core list | grep -q '^esp32:esp32'; then
  log "Installation du core esp32:esp32… (si le réseau coupe, relance la commande)"
  arduino-cli core update-index
  arduino-cli core install esp32:esp32
fi

# ============ Build AVR (UNO/Proteus) ============
build_uno() {
  log "[AVR] Compilation pour Arduino Uno (Proteus)"
  arduino-cli compile \
    --fqbn arduino:avr:uno \
    --export-binaries \
    --output-dir "$BIN_UNO" \
    "$SKETCH_ROOT"

  # Trouver les sorties
  ELF=$(ls "$BIN_UNO"/*.ino.elf 2>/dev/null | head -n1 || true)
  HEX=$(ls "$BIN_UNO"/*.ino.hex 2>/dev/null | head -n1 || true)
  [[ -n "${ELF:-}" ]] && log "[AVR] ELF: $ELF"
  [[ -n "${HEX:-}" ]] && log "[AVR] HEX: $HEX"
  [[ -z "${ELF:-}" || -z "${HEX:-}" ]] && warn "[AVR] Attention: fichiers ELF/HEX non trouvés dans $BIN_UNO"
}

# ============ Build ESP32 ============
build_esp32() {
  log "[ESP32] Compilation (binaire pour carte réelle)"
  arduino-cli compile \
    --fqbn esp32:esp32:esp32 \
    --export-binaries \
    --output-dir "$BIN_ESP32" \
    "$SKETCH_ROOT"

  BOOT=$(ls "$BIN_ESP32"/*bootloader*.bin 2>/dev/null | head -n1 || true)
  PART=$(ls "$BIN_ESP32"/*partitions*.bin  2>/dev/null | head -n1 || true)
  APP=$(ls "$BIN_ESP32"/*.ino.bin          2>/dev/null | head -n1 || true)

  [[ -n "${BOOT:-}" ]] || err "[ESP32] bootloader.bin manquant dans $BIN_ESP32"
  [[ -n "${PART:-}" ]] || err "[ESP32] partitions.bin manquant dans $BIN_ESP32"
  [[ -n "${APP:-}"  ]] || err "[ESP32] app .ino.bin manquant dans $BIN_ESP32"

  log "[ESP32] Bootloader: $BOOT"
  log "[ESP32] Partitions: $PART"
  log "[ESP32] App:        $APP"
}

# ============ Flash ESP32 ============
flash_esp32() {
  # Choisir le port si absent
  if [[ -z "$PORT" ]]; then
    PORT=$(ls /dev/ttyUSB* 2>/dev/null | head -n1 || true)
    [[ -z "$PORT" ]] && err "Aucun /dev/ttyUSB* trouvé. Spécifie PORT=/dev/ttyUSB0 ou attache l’ESP32 à WSL."
  fi
  log "[ESP32] Port série: $PORT  | Baud: $BAUD"

  # Prépare esptool dans un venv dédié (propre)
  VENV="$HOME/.venvs/esptool"
  if [[ ! -x "$VENV/bin/esptool.py" ]]; then
    log "Création du venv esptool ($VENV)…"
    python3 -m venv "$VENV"
    "$VENV/bin/python" -m pip install --upgrade pip >/dev/null
    "$VENV/bin/python" -m pip install esptool pyserial >/dev/null
  fi
  ESPTOOL="$VENV/bin/esptool.py"

  # Re-détecter les fichiers, au cas où on ne vient que flasher
  BOOT=$(ls "$BIN_ESP32"/*bootloader*.bin 2>/dev/null | head -n1 || true)
  PART=$(ls "$BIN_ESP32"/*partitions*.bin  2>/dev/null | head -n1 || true)
  APP=$(ls "$BIN_ESP32"/*.ino.bin          2>/dev/null | head -n1 || true)
  [[ -n "${BOOT:-}" && -n "${PART:-}" && -n "${APP:-}" ]] || err "Fichiers binaires ESP32 manquants. Lance d’abord le build."

  log "[ESP32] Flash en cours… (si ça n’accroche pas, maintiens BOOT puis relâche)"
  "$ESPTOOL" --chip esp32 --port "$PORT" --baud "$BAUD" \
    --before default_reset --after hard_reset \
    write_flash -z \
      0x1000  "$BOOT" \
      0x8000  "$PART" \
      0x10000 "$APP"

  log "[ESP32] Flash terminé ✅"
}

# ============ Orchestration ============
case "$TARGET" in
  all)
    build_uno
    build_esp32
    [[ "$DO_FLASH" == "yes" ]] && flash_esp32
    ;;
  uno)
    build_uno
    ;;
  esp32)
    build_esp32
    [[ "$DO_FLASH" == "yes" ]] && flash_esp32
    ;;
  *)
    err "Usage: $0 [all|uno|esp32|--flash]\nExemples:\n  $0            # build UNO + ESP32\n  $0 uno        # build uniquement UNO\n  $0 esp32      # build uniquement ESP32\n  PORT=/dev/ttyUSB0 $0 --flash   # build tout + flash ESP32"
    ;;
esac
