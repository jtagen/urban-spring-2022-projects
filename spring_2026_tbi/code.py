import time
import board
import busio
import digitalio
import displayio
import terminalio
import neopixel
from adafruit_display_text import label
#from adafruit_mpr121 import MPR121
from adafruit_magtag.magtag import MagTag
from adafruit_bitmap_font import bitmap_font
from adafruit_mcp230xx.mcp23017 import MCP23017

def is_touched(sensor_index):
    return pins[sensor_index - 1].value  # MCP23017 input (True when touched)



# Initialize MagTag
magtag = MagTag()

# Initialize I2C and MPR121
i2c = busio.I2C(board.SCL, board.SDA)
mcp = MCP23017(i2c)  # MCP23017
#mpr121 = MPR121(i2c)
#pixels = magtag.peripherals.neopixels
#pixels.brightness = 0.3



pixels = neopixel.NeoPixel(board.D10, 60, brightness=0.3, auto_write=False)
#Erase the board
pixels.fill((0, 0, 0))
pixels.show()



font = bitmap_font.load_font("fonts/spleen-16x32.bdf")




# This is the sequence that must be touched. Neopixel strip # (1-based counting) followed by sensor number (also 1-based counting)
pattern = [
    [56,1], 
    [8,12], 
    [31,8], 
    [45,2], 
    [17,9], 
    [35,7], 
    [49,3], 
    [24,11], 
    [52,4],
    [21,10], 
#    [41,5], 
#    [38,6],
    ]


pins = []
for i in range(12):
    pin = mcp.get_pin(i)
    pin.direction = digitalio.Direction.INPUT
    pins.append(pin)

text_area = label.Label(font, text="", color=0x000000, x=15, y=30)
magtag.splash.append(text_area)



while True:
    pixels.fill((0,0,0))
    for pixel,position in pattern:
        pixels[pixel-1] = (0,255,0)
    pixels.show()




    text_area.text="Choose Pattern\n\nA             B"
    magtag.refresh()

    while magtag.peripherals.button_a_pressed is False and magtag.peripherals.button_d_pressed is False:
        time.sleep(.01)


    text_area.text = "Ready to Start"
    magtag.refresh()

    if magtag.peripherals.button_a_pressed:
        direction = -1
    else:
        direction = 1

    pixels.fill((0,0,0))
    pixels.show()

    # Wait for first correct touch
    first_pixel, first_sensor = pattern[::direction][0]

    pixels[first_pixel] = (255,255,255)
    pixels.show()
    while not is_touched(first_sensor):
        pass


    # Start timing
    start_time = time.monotonic()

    # Go through the full pattern
    for pixel_index, sensor_index in pattern[::direction]:
        # Light up current target
        pixels.fill((0, 0, 0))
        pixels[pixel_index - 1] = (255, 255, 255)  # white target
        pixels.show()

        # Wait until correct sensor is touched
        while not is_touched(sensor_index):
            pass

        # Small debounce delay
        time.sleep(0.05)

    # Stop timing
    end_time = time.monotonic()
    total_time = end_time - start_time

    # Clear LEDs
    pixels.fill((0,0,0))
    for pixel,position in pattern:
        pixels[pixel-1] = (255,0,0)
    pixels.show()

    # Display result


    if total_time < 20:
        result = "Likely no TBI"
    elif total_time < 25:
        result = "Suspect mild TBI"
    else:
        result = "Suspect\nmoderate TBI"

    text_area.text = result + "\nTime: {:.2f} sec".format(total_time)
    magtag.refresh()

    """
    15-20 Likely no TBI
    20-25 Suspected mild TBI
    25-30 Moderate TBI

    """
    print("Total time:", total_time)

    while magtag.peripherals.button_a_pressed is False and magtag.peripherals.button_b_pressed is False and magtag.peripherals.button_c_pressed is False and magtag.peripherals.button_d_pressed is False:
        time.sleep(.01)




