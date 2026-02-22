# ==============================
# Auto attach ReadPi to WSL
# Run as Administrator
# ==============================

Write-Host "🔍 Searching for RP2040 device..."

# Cherche un périphérique Raspberry Pi RP2 (VID:PID 2e8a:0005 ou 0003)
$device = usbipd list | Select-String "2e8a"

if (-not $device) {
    Write-Host "❌ No RP2040 device found."
    exit
}

# Extraire BUSID (première colonne)
$busid = ($device -split '\s+')[0]

Write-Host "✅ Device found on BUSID: $busid"

Write-Host "🔗 Attaching to WSL..."

usbipd bind --busid $busid 2>$null
usbipd attach --wsl --busid $busid --force

Write-Host "🚀 Attached successfully!"