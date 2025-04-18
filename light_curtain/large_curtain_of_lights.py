import time
import board
import busio
import neopixel
import terminalio
from adafruit_magtag.magtag import MagTag
from adafruit_mpr121 import MPR121
from adafruit_display_text import label

magtag = MagTag()
pixels = neopixel.NeoPixel(board.A1, 90)

# Touch number & LED number


i2c = busio.I2C(board.SCL, board.SDA)
mpr121a = MPR121(i2c,0x5a)
mpr121b = MPR121(i2c,0x5b)


# sensor board, input on MPR121, led #
buttons = [[mpr121b,11,6], [mpr121b,10,7],[mpr121b,9,8],[mpr121a,7,17]]


text_area = label.Label(terminalio.FONT, text="Please touch \nthe circles", color=0x000000, x=20, y=40)
magtag.splash.append(text_area)


def ShowLED(led_number):
    for i in range(90):
        pixels[i] = (0,0,0) #blank out all pixels
    pixels[led_number] = (255,0,0) #make the one we want lit be be 100% red

while True:
    text_area.text = "Please touch \nthe circles"
    magtag.refresh()
    ShowLED(buttons[0][2])

    current_position = 0
    sensor_pressed = False

    while current_position < len(buttons):
        if buttons[0][0][buttons[0][1]].value:
            sensor_pressed = True
            start_time = time.time() #start timer when first input is pressed
        ShowLED(buttons[current_position][2])
        while not buttons[current_position][0][buttons[current_position][1]].value:   # do nothing until the LED we want has been pressed
            pass
        current_position = current_position + 1

    duration = time.time() - start_time
    ShowLED(26)

    if duration <= 12:
        text_area.text = f"Mild - Total time:\n{duration} seconds"
    elif duration in range (13,21):
        text_area.text = f"Moderate - Total \ntime: {duration} seconds"
    else:
        text_area.text = f"Severe - Total \ntime: {duration} seconds"

    magtag.refresh()
    time.sleep(5)
    for i in range(90):
        pixels[i] = (0,0,0)





