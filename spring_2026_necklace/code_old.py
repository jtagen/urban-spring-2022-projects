import time
import busio
import board
import displayio
import terminalio
from adafruit_display_text import label
import adafruit_mpr121
import adafruit_displayio_sh1107 

i2c = board.STEMMA_I2C() 


# Note that multi-touch items *must* be first in this list
messages = [
    {"buttons": {2, 10}, "text": "Need support toy", "color": "green"},
    {"buttons": {3, 8}, "text": "Need a break", "color": "green"},
    {"buttons": {5, 6}, "text": "Need to be alone", "color": "green"},
    {"buttons": {0}, "text": "", "color": "blue"},
    {"buttons": {1}, "text": "Restroom?", "color": "blue"},
    {"buttons": {2}, "text": "Water?", "color": "blue"},
    {"buttons": {3}, "text": "Need support", "color": "blue"},
    {"buttons": {5}, "text": "Want pen\n+ paper", "color": "blue"},
    {"buttons": {6}, "text": "Don't understand", "color": "blue"},
    {"buttons": {8}, "text": "All good", "color": "blue"},
    {"buttons": {10}, "text": "Don't feel well", "color": "green"},
    {"buttons": {11}, "text": "I'm uncomfortable", "color": "green"},
]


# Initialize I2C and MPR121
displayio.release_displays()
i2c = board.STEMMA_I2C()  # uses board.SCL and board.SDA
display_bus = displayio.I2CDisplay(i2c, device_address=0x3c)
mpr121 = adafruit_mpr121.MPR121(i2c)

WIDTH = 128
HEIGHT = 64
BORDER = 1
display = adafruit_displayio_sh1107.SH1107(
    display_bus, width=WIDTH, height=HEIGHT, rotation=90
)


def ShowText(t, s=2):
    splash = displayio.Group()
    display.root_group = splash

    text_area = label.Label(
        terminalio.FONT, text=t, scale=s, color=0xFFFFFF, x=5, y=15
    )
    splash.append(text_area)

     
ShowText("Ready",2)
time.sleep(2)
ShowText("");

hold_start = None
sos_mode = False
last_flash = 0
flash_on = False

while True:
    touch_state = [mpr121[i].value for i in range(0, 12)]  # Read touch inputs

    # Special case for alarm
    if touch_state[2] and touch_state[9]: # buttons for SOS mode
        if hold_start is None:
            print("SOS Timing Start")
            hold_start = time.time()
        elif time.time() - hold_start >= 3: # 3 seconds to activate
            print("SOS start")
            sos_mode = not sos_mode
            hold_start = None
    else:
        hold_start = None

    if sos_mode:
        print("In SOS mode")
        if time.time() - last_flash > 1.0:  # flash interval
            last_flash = time.time()
            flash_on = not flash_on
            if flash_on:
                ShowText("SOS")
            else:
                ShowText("")
    if hold_start or sos_mode:
        continue # Don't check buttons below

    if any(x == 1 for x in touch_state):
        time.sleep(0.3)
        print("Touch detected")
        for i in range(0,12):
            print(f"{i} {touch_state[i]} ", end="")
        print("")
        # see if one matches
        for message in messages:
            if all(touch_state[i] == 1 for i in message["buttons"]):
                ShowText(message["text"],2)
                time.sleep(5)
                ShowText("")


