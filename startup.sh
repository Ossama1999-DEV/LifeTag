#!/bin/bash
set -e  # Stop si une commande échoue

# === Vérification environnement ===
echo "[INFO] Vérification des dépendances..."

for cmd in cmake make g++; do
    if ! command -v $cmd &> /dev/null; then
        echo "[ERREUR] $cmd n'est pas installé. Installez-le avant de continuer."
        exit 1
    fi
done

echo "[OK] Environnement compilateur prêt."

# === Compilation ===
echo "[INFO] Nettoyage et compilation..."
cd /mnt/c/Users/dbibi/OneDrive/Bureau/CESI/2025-2026/CRI/LifeTag

# Créer build s'il n'existe pas
mkdir -p build
cd build

# Nettoyer
rm -rf *

# Génération & compilation
cmake ..
make -j$(nproc)

echo "[OK] Compilation terminée."

# === Création dossier driver ===
echo "[INFO] Préparation du dossier driver/"
cd /mnt/c/Users/dbibi/OneDrive/Bureau/CESI/2025-2026/CRI/LifeTag
mkdir -p driver

# Copier les binaires et libs
cp build/main/LifeTagApp driver/ 2>/dev/null || true
cp build/lib/*.a driver/ 2>/dev/null || true
cp build/lib/*.so driver/ 2>/dev/null || true
cp build/test/LifeTagTests driver/ 2>/dev/null || true

echo "[OK] driver/ contient :"
ls -lh driver/

# ╭──────────────────────────────╮
# │ DOC & CHANGELOG              │
# ╰──────────────────────────────╯
echo "📝 Génération du CHANGELOG..."
git log --pretty=format:"- %s" > CHANGELOG.md || true

echo "[INFO] Script terminé avec succès."