from nfc import NFC
from time import sleep

nfc = NFC(9600)

text = "LIFETAG:001"
start_page = 15

hex_data = text.encode("ascii").hex()

for i in range(0, len(hex_data), 8):
    page = start_page + (i // 8)
    chunk = hex_data[i:i+8]

    # padding manuel MicroPython
    while len(chunk) < 8:
        chunk += "0"

    print("Write page", page, ":", chunk)
    nfc.Data_write(chunk, str(page))
    sleep(0.3)

print("DONE")