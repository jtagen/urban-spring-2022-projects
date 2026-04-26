import time
import board
import busio
import displayio
import terminalio
from adafruit_display_text import label
from adafruit_mpr121 import MPR121
import adafruit_ssd1306

i2c = busio.I2C(board.SCL, board.SDA)
display = adafruit_ssd1306.SSD1306_I2C(128, 64, i2c)
display.fill(0)
display.show()

i2c = busio.I2C(board.SCL, board.SDA)
mpr121 = MPR121(i2c)

display = board.DISPLAY
main_group = displayio.Group()

text_area = label.Label(terminalio.FONT, text="Waiting for touch...", color=0xFFFFFF, x=10, y=20)
main_group.append(text_area)
display.show(main_group)

sensor_messages = {
    1: "Hi my name is _____",
    2: "Can I use the bathroom?",
    3: "Can I have this?",
    4: "Where is ____?",  
    5: "Excuse me!",
    6: "Can you get that?",
    7: "Can you get that?",
    8: "Thank you!",
    9: "Please",
}

toggle_state = False

while True:
    touch_state = [mpr121[i].value for i in range(0, 10)]  

    if touch_state[0]:  
        toggle_state = not toggle_state
        text_area.text = "ON" if toggle_state else "OFF"
    
    elif touch_state[0] and touch_state[1]:  
        text_area.text = "I need help"
    
    elif touch_state[6] and touch_state[7]:  
        text_area.text = "What time is it?"
    
    else:
        for i in range(1, 10):  
            if touch_state[i]:  
                text_area.text = sensor_messages.get(i, "Touch detected!")
                break  

    display.refresh()
    time.sleep(0.5)  

    if not any(touch_state):  
        text_area.text = "Waiting for touch..."
        display.refresh()

    time.sleep(0.1)  
