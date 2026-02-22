from machine import Pin, SPI
import st7789
import vga1_bold_16x32 as font
import time

# SPI configuration (ReadPi)
spi = SPI(1, baudrate=40000000, polarity=1, phase=1,
          sck=Pin(10), mosi=Pin(11))

tft = st7789.ST7789(
    spi,
    240,
    240,
    reset=Pin(12, Pin.OUT),
    cs=Pin(9, Pin.OUT),
    dc=Pin(8, Pin.OUT),
    backlight=Pin(13, Pin.OUT),
    rotation=0
)

tft.init()

# Effacer écran
tft.fill(st7789.BLACK)

# Afficher texte
tft.text(font, "LifeTag", 40, 80, st7789.GREEN)
tft.text(font, "READY", 40, 120, st7789.WHITE)

while True:
    pass