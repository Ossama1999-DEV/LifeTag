from machine import Pin, SPI
import st7789
import vga1_bold_16x32 as font

class Display:
    def __init__(self, *, sck, mosi, rst, cs, dc, bl, width=240, height=240, rotation=0):
        self.spi = SPI(1, baudrate=40_000_000, polarity=1, phase=1,
                       sck=Pin(sck), mosi=Pin(mosi))
        self.tft = st7789.ST7789(
            self.spi, width, height,
            reset=Pin(rst, Pin.OUT),
            cs=Pin(cs, Pin.OUT),
            dc=Pin(dc, Pin.OUT),
            backlight=Pin(bl, Pin.OUT),
            rotation=rotation
        )
        self.tft.init()

    def clear(self):
        self.tft.fill(st7789.BLACK)

    def write(self, text, x=0, y=0, color=st7789.WHITE):
        self.tft.text(font, text, x, y, color)