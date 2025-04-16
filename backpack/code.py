import time
import board
import busio
import displayio
import terminalio
import neopixel
from adafruit_display_text import label
from adafruit_mpr121 import MPR121
from adafruit_magtag.magtag import MagTag
from adafruit_bitmap_font import bitmap_font


# Initialize MagTag
magtag = MagTag()

# Initialize I2C and MPR121
i2c = busio.I2C(board.SCL, board.SDA)
mpr121 = MPR121(i2c)
pixels = magtag.peripherals.neopixels
pixels.brightness = 0.3

"""
messages = {
    2: "Pin 1",
    2: "Hi my name\n is _____",
    3: "Yes", #make yes with all green lights
    4: "No",#make no with all red lights
    5: "Where is ____?",  #make for two fingers 
    6: "Excuse me!",
    7: "Can you get that?",
    8: "Thank you!",
    9: "My name is Chad",
    10: "You're welcome",
}
"""


messages = {
    0: "Unassigned 0",
    1: "Hi, My name\nis Chad",
    2: "Yes",
    3: "No",
    4: "Can you help me?",
    5: "Pin 5",
    6: "I'll text you",
    7: "Thank You",
    8: "Tall Vanilla\nLatte Please",
    9: "Where is\nthe bathroom?",
    10: "Unassigned 10",
}


font = bitmap_font.load_font("fonts/spleen-16x32.bdf")
#font = terminalio.FONT

toggle_state = False  # Initial toggle state

# Set up the display
text_area = label.Label(font, text="", color=0x000000, x=15, y=30)
magtag.splash.append(text_area)
magtag.refresh()

while True:
    touch_state = [mpr121[i].value for i in range(10)] 
    pixels.fill((0,0,0))
    pixels.show()    
    if any(touch_state):
        # Short delay to allow people to put all their fingers on the sensor, then re-read
        # Tone to indicate a successful read

        for i in range(0, 10):
            if touch_state[i]: 
                magtag.peripherals.play_tone(5000, .1)
                text_area.text = messages.get(i)
        
                if i == 2:
                    pixels.fill((0,255,0))
                    pixels.show()
                elif i ==3:
                    pixels.fill((255,0,0))
                    pixels.show()

        magtag.refresh()
        #delay before showing something else
        time.sleep(2)
        text_area.text = ""
        pixels.fill((0,0,0))
        pixels.show()
        magtag.refresh()

