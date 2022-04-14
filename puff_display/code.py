import time
import board
import adafruit_mprls
from adafruit_magtag.magtag import MagTag
i2c = board.I2C()
mpr = adafruit_mprls.MPRLS(i2c, psi_min=0, psi_max=25)
BASE_PSI = mpr.pressure
POS_OFFSET = 2 #pressure change to trigger "puff"
NEG_OFFSET = -1 #negative pressure change to trigger "sip" 
PUFF_WAIT_TIME = 2 #minimum wait time for another puff, seconds
puff_state = "neutral"
puff_count = 0;
messages = {1:"Hello, my name\nis Samantha",2:"Yes\n  Thank You",3:"No,\n  Thank You",
4: "Water, please", 5: "Help,\nAssistance\n   needed"}



magtag = MagTag()

magtag.add_text(
    text_position=(
        20,
        (magtag.graphics.display.height // 2) - 1,
    ),
    text_scale=3,
)

puff_end_time = 0

def HandlePuffCount(n):
	print("Handling puff count of " + str(n))
	
	magtag.set_text(messages.get(n))
	magtag.peripherals.neopixel_disable = False
	for i in range(n):
		magtag.peripherals.neopixels.fill((0,255,0))
		magtag.peripherals.play_tone(1500, 0.25)
		time.sleep(.01)

	magtag.peripherals.neopixel_disable = True






while True:

	pressure = mpr.pressure
	#print(time.monotonic())
	#print(pressure)
	if pressure < BASE_PSI + NEG_OFFSET and puff_state != "neutral":
		puff_state = "neutral"
		puff_count = 0;
		print("Negative reset")

	if pressure > BASE_PSI + POS_OFFSET and puff_state == "neutral":
		puff_state = "positive"
		puff_count = puff_count + 1
		print("Puff started, count is " + str(puff_count))

	if pressure < BASE_PSI + POS_OFFSET and puff_state == "positive":
		puff_state = "neutral"
		puff_end_time = time.monotonic()
		print("Puff ended")

	if (puff_count > 0 and puff_state == "neutral" and time.monotonic() > (puff_end_time + PUFF_WAIT_TIME)):
		print("Done, puff count was " + str(puff_count))
		HandlePuffCount(puff_count)
		puff_count = 0

	time.sleep(.01)




