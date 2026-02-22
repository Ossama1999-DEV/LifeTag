from machine import Pin
from time import sleep

from lifetag.pins import Pins
from lifetag.display.st7789_display import Display

def run():
    led = Pin(Pins.LED, Pin.OUT)

    disp = Display(
        sck=Pins.TFT_SCK, mosi=Pins.TFT_MOSI,
        rst=Pins.TFT_RST, cs=Pins.TFT_CS, dc=Pins.TFT_DC, bl=Pins.TFT_BL
    )

    disp.clear()
    disp.write("LifeTag", 40, 80)
    disp.write("READY",  40, 120)

    while True:
        led.toggle()
        sleep(0.5)