#!/bin/bash
set -e  # Stop si une commande échoue
# Test de connectivité réseau
sudo ip link set enx00249b6a56b6 up
sudo ip addr add 10.0.0.1/24 dev enx00249b6a56b6
ping 10.0.0.2

