import time
import board
import busio
import displayio
import terminalio
from adafruit_display_text import label
import adafruit_mpr121
#import adafruit_displayio_sh1107 


tag_width = 18


# Note that multi-touch items *must* be first in this list
messages = [
    {"buttons": {2, 10}, "text": "Need\nsupport toy", "color": 0x00FF00, "size":3},
    {"buttons": {3, 8}, "text": "Need a\nbreak", "color": 0x00FF00, "size":4},
    {"buttons": {5, 6}, "text": "Need to\nbe alone", "color": 0x00FF00, "size":4},
    {"buttons": {0}, "text": "", "color": 0x0000FF, "size":4},
    {"buttons": {1}, "text": "Restroom?", "color": 0x0000FF, "size":4},
    {"buttons": {2}, "text": "Water?", "color": 0x0000FF, "size":4},
    {"buttons": {3}, "text": "Need\nsupport", "color": 0x0000FF, "size":4},
    {"buttons": {5}, "text": "Want pen\n+ paper", "color": 0x0000FF, "size":4},
    {"buttons": {6}, "text": "Don't\nunderstand", "color": 0x0000FF, "size":4},
    {"buttons": {8}, "text": "All good", "color": 0x0000FF, "size":4},
    {"buttons": {10}, "text": "Don't\nfeel well", "color": 0x00FF00, "size":4},
    {"buttons": {11}, "text": "I'm\nuncomfortable", "color": 0x00FF00, "size":3},
]


# Initialize I2C and MPR121
i2c = board.I2C()  # uses board.SCL and board.SDA
mpr121 = adafruit_mpr121.MPR121(i2c)
display = board.DISPLAY

WIDTH = 128
HEIGHT = 64
BORDER = 1


def ShowText(t, scale=2, color=0xFFFFFF):
    splash = displayio.Group()
    
    display.root_group = splash

    text_area = label.Label(
        terminalio.FONT, text=t, scale=scale, color=color, x=5, y=15
    )
    splash.append(text_area)



ShowText("ready",color=0xFFFFFF)
time.sleep(2)
ShowText("");

hold_start = None
sos_mode = False
last_flash = 0
flash_on = False

while True:
    touch_state = [mpr121[i].value for i in range(0, 12)]  # Read touch inputs

    if any(x == 1 for x in touch_state):
        time.sleep(0.3)



    touch_state = [mpr121[i].value for i in range(0, 12)]  # Read touch inputs

    print("Touch detected")
    for i in range(0,12):
        print(f"{i} {touch_state[i]} ", end="")
    print("")

    # Special case for alarm
    if touch_state[1] and touch_state[11]: # buttons for SOS mode
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
                ShowText("SOS",scale=4,color=0xFF0000)
            else:
                ShowText("")
    if hold_start or sos_mode:
        continue # Don't check buttons below

    if not any(x == 1 for x in touch_state):
    	continue

    # see if one matches
    for message in messages:
        if all(touch_state[i] == 1 for i in message["buttons"]):
            ShowText(message["text"],scale=message["size"], color=message["color"])
            time.sleep(5)
            ShowText("")


