# Quick test file to show data on screen

import board
import time
import adafruit_lsm303_accel
import terminalio
import displayio
from adafruit_display_text import label
import adafruit_ahtx0
import analogio

i2c = board.I2C()  # uses board.SCL and board.SDA
accel_sensor = adafruit_lsm303_accel.LSM303_Accel(i2c)
temp_sensor = adafruit_ahtx0.AHTx0(board.I2C())

FONTSCALE = 2
TEXT_COLOR = 0xFFFF00
pulse_pin = analogio.AnalogIn(board.A0)  #575 - 53023

display = board.DISPLAY
max_x = 0
max_y = 0
max_z = 0

while True:

    splash = displayio.Group()
    acc_x, acc_y, acc_z = accel_sensor.acceleration
    if abs(acc_x) > max_x:
        max_x = abs(acc_x)
    if abs(acc_y) > max_y:
        max_y = abs(acc_y)
    if abs(acc_z) > max_z:
        max_z = abs(acc_z)

    pulse_reading = 1 - (pulse_pin.value - 575) / (53023-575)
    text = '{0:3.3f},{1:3.3f},{2:3.3f}\n{3:3.3f},{4:3.3f},{5:3.3f}\n{6:0.1f}C, {7:0.1f}% RH\n{8}'.format(acc_x, acc_y, acc_z, max_x, max_y, max_z,temp_sensor.temperature, temp_sensor.relative_humidity, pulse_reading)
    text_area = label.Label(terminalio.FONT, text=text, color=TEXT_COLOR)
    text_width = text_area.bounding_box[2] * FONTSCALE
    text_group = displayio.Group(
        scale=FONTSCALE,
        x=display.width // 2 - text_width // 2,
        y=10,
    )
    text_group.append(text_area)  # Subgroup for text scaling
    splash.append(text_group)
    display.root_group = splash

    time.sleep(0.1)

