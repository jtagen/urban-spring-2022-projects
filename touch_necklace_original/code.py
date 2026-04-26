import time
import board
import busio
import displayio
import terminalio
from adafruit_display_text import label
import adafruit_mpr121
import adafruit_displayio_sh1107 

messages = {
    2: "Hi my name\nis _____",
    3: "Restroom \nplease",
    4: "Can I \nhave this?",
    5: "Where is \n____?",  
    6: "Excuse me!",
    7: "Can you \nget that?",
    8: "Thank you!",
    9: "My name \nis Chad",
    10: "You're welcome",
}

# Initialize I2C and MPR121
displayio.release_displays()
i2c = board.I2C()  # uses board.SCL and board.SDA
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

     
ShowText("Waiting",2)
time.sleep(2)
ShowText("")



while True:
    touch_state = [mpr121[i].value for i in range(1, 11)]  # Read touch inputs

    if touch_state[1] and touch_state[2]:  
        ShowText("I need \nhelp")
    
    elif touch_state[6] and touch_state[7]:  
        ShowText("What time is it?")
    
    else:
        for i in range(2, 11):
            if touch_state[i-1]: 
                ShowText(messages.get(i, ""))
                break