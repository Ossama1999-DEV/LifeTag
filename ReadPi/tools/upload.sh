#!/usr/bin/env bash
set -euo pipefail

PORT="/dev/ttyACM0"

sudo chmod 666 /dev/ttyACM0

# Copie ta lib
mpremote connect "$PORT" fs mkdir lifetag || true
mpremote connect "$PORT" fs cp -r src/lifetag :/

# Copie vendor
mpremote connect "$PORT" fs cp vendor/st7789.py :
mpremote connect "$PORT" fs cp vendor/vga1_bold_16x32.py :

# Copie main.py
mpremote connect "$PORT" fs cp main.py :

# Reset
mpremote connect "$PORT" reset
echo "✅ Uploaded & reset"