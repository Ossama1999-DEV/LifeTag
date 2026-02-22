from machine import Pin, SPI, PWM
import time

import st7789
import vga1_bold_16x32 as font
from nfc import NFC

# ----------------------------
# CONFIG (basé repo sbcshop)
# ----------------------------
BAUDRATE = 9600

# On lit RAW (NTAG = 4 bytes/page) — comme l'exemple qui lit/écrit page 15
PAGES_TO_READ = [15, 16, 17, 18]

GOOD_PREFIX = "TAG:"  # tag "BON" si le texte RAW commence par ça

# Buzzer onboard ReadPi: GP15 (repo)
BUZZER_PIN = 15
BEEP_FREQ = 1865
BEEP_ON = 0.12
BEEP_OFF = 0.10

# LED : la seule sûre “onboard” = GP25 ("LED" fonctionne aussi)
LED_PIN = "LED"

# ----------------------------
# Buzzer (repo style)
# ----------------------------
buzzer = PWM(Pin(BUZZER_PIN))

def playtone(freq=BEEP_FREQ):
    buzzer.duty_u16(5000)   # repo uses 5000
    buzzer.freq(freq)

def bequiet():
    buzzer.duty_u16(0)

def beep(n):
    for _ in range(n):
        playtone()
        time.sleep(BEEP_ON)
        bequiet()
        time.sleep(BEEP_OFF)

# ----------------------------
# Helpers read/parse
# ----------------------------
def hex_to_bytes(hex_str):
    try:
        return bytes.fromhex(hex_str)
    except Exception:
        return b""

def bytes_to_ascii_clean(b):
    # garde imprimables, remplace le reste par espace
    s = "".join(chr(x) if 32 <= x <= 126 else " " for x in b)
    # nettoie espaces multiples
    return " ".join(s.strip().split())

def read_pages_hex(nfc):
    chunks = []
    for p in PAGES_TO_READ:
        rec = nfc.data_read(str(p))
        if rec is None or rec == "Card not detect":
            return None
        chunks.append(rec)          # 8 hex chars = 4 bytes
    return "".join(chunks)

def looks_readable(text):
    # “mal lu” si vide, ou trop de zéros
    if not text:
        return False
    if text.replace("0", "") == "":
        return False
    return True

# ----------------------------
# Display init (repo pins)
# ----------------------------
spi = SPI(1, baudrate=40000000, sck=Pin(10), mosi=Pin(11))
tft = st7789.ST7789(
    spi, 240, 240,
    reset=Pin(12, Pin.OUT),
    cs=Pin(9, Pin.OUT),
    dc=Pin(8, Pin.OUT),
    backlight=Pin(13, Pin.OUT),
    rotation=1
)
tft.init()

# ----------------------------
# DISPLAY HELPERS (PRO LAYOUT)
# ----------------------------
HEADER_Y = 5
BODY_Y = 80
RESULT_Y = 170

def center_x(text):
    # écran 240 px ; font 16 px / caractère
    return max(0, (240 - len(text) * 16) // 2)

def header():
    # Clear total puis header compact
    tft.fill(0)
    tft.text(font, "LIFETAG", 5, 5, st7789.CYAN)
    tft.text(font, "SAFYRA SYS", 5, 30, st7789.CYAN)

def show_scan():
    header()
    tft.text(font, "SCAN TAG", center_x("SCAN TAG"), 150, st7789.YELLOW)

def show_result(text, ok):
    header()

    # Ligne "LU:" plus haut
    tft.text(font, "LU:", 5, 80, st7789.WHITE)

    if not text:
        text = "NO DATA"

    # 2 lignes max
    line1 = text[:12]
    line2 = text[12:24] if len(text) > 12 else ""

    tft.text(font, line1, center_x(line1), 85, st7789.WHITE)
    if line2:
        tft.text(font, line2, center_x(line2), 125, st7789.WHITE)

    if ok:
        tft.text(font, "BON", center_x("BON"), RESULT_Y, st7789.GREEN)
    else:
        tft.text(font, "MAUVAIS", center_x("MAUVAIS"), RESULT_Y, st7789.RED)

# ----------------------------
# Init NFC + LED
# ----------------------------
nfc = NFC(BAUDRATE)
led = Pin(LED_PIN, Pin.OUT)
led.value(0)

# ----------------------------
# Main loop
# ----------------------------
show_scan()
last_hex = None
cooldown_s = 0.8
last_t = 0.0

while True:
    hx = read_pages_hex(nfc)

    if hx is None:
        # pas de carte
        if last_hex is not None:
            show_scan()
            last_hex = None
        time.sleep(0.2)
        continue

    now = time.time()
    if hx == last_hex and (now - last_t) < cooldown_s:
        time.sleep(0.1)
        continue

    last_hex = hx
    last_t = now

    raw = hex_to_bytes(hx)
    txt = bytes_to_ascii_clean(raw)

    # “mal lu” = rien de lisible
    if not looks_readable(txt):
        show_result(hx[:24], ok=True)  # affiche un bout d’hex si pas lisible
        beep(1)                         # bip
        continue

    # “bon” = commence par LIFETAG:
    ok = txt.startswith(GOOD_PREFIX)

    show_result(txt, ok=ok)

    if ok:
        beep(3)                         # bip bip bip
        led.value(1); time.sleep(0.1); led.value(0)
    else:
        beep(1)                         # bip bip