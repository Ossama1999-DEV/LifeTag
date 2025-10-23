#!/bin/bash
set -e  # Stop si une commande échoue

# ╭───────────────────────────────────────────────╮
# │  CHECK ENVIRONNEMENT ARDUINO / ESP32          │
# ╰───────────────────────────────────────────────╯
echo "[INFO] Vérification de l'environnement Arduino-CLI..."

if ! command -v arduino-cli &>/dev/null; then
    echo "[ERREUR] arduino-cli n'est pas installé."
    echo "➡ Installez-le avec :"
    echo "   curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh"
    exit 1
fi

# Vérification des plateformes installées
echo "[INFO] Vérification des plateformes nécessaires..."
if ! arduino-cli core list | grep -q "esp32"; then
    echo "[WARN] ESP32 core non trouvé, installation en cours..."
    arduino-cli core update-index
    arduino-cli core install esp32:esp32
fi

if ! arduino-cli core list | grep -q "arduino:avr"; then
    echo "[WARN] Core AVR (Arduino UNO) non trouvé, installation en cours..."
    arduino-cli core update-index
    arduino-cli core install arduino:avr
fi

echo "[OK] Environnement Arduino prêt."

# === Compilation ===
echo "[INFO] Nettoyage et compilation..."

cd LifeTag_Arduino/main
arduino-cli compile --fqbn esp32:esp32:esp32 --output-dir ../bin_ESP32 --verbose
arduino-cli compile --fqbn arduino:avr:uno --output-dir ../build_Proteus --verbose

echo "[OK] Compilation terminée."

cd ../../
# === Création dossier driver ===
echo "[INFO] Préparation du dossier driver/"
mkdir -p driver_ESP32
mkdir -p driver_Proteus

# Copier les binaires et libs
cp LifeTag_Arduino/bin_ESP32/* driver_ESP32/ 2>/dev/null || true
cp LifeTag_Arduino/build_Proteus/* driver_Proteus/ 2>/dev/null || true

echo "[OK] driver_ESP32/ contient :"
ls -lh driver_ESP32/
echo "[OK] driver_Proteus/ contient :"
ls -lh driver_Proteus/

# ╭──────────────────────────────╮
# │ DOC & CHANGELOG              │
# ╰──────────────────────────────╯
echo "📝 Génération du CHANGELOG..."
git log --pretty=format:"- %s" > CHANGELOG.md || true

echo "[INFO] Script terminé avec succès."