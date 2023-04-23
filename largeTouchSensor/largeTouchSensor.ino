#define DEBUG true
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"
#include "Adafruit_ThinkInk.h"

#define EPD_CS      10
#define EPD_DC      9
#define SRAM_CS     7
#define EPD_RESET   -1 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)


// 2.13" Monochrome displays with 250x122 pixels and SSD1680 chipset
ThinkInk_213_Mono_BN display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);
Adafruit_NeoPixel pixels(8, 13, NEO_GRB + NEO_KHZ800);
Adafruit_MPR121 cap = Adafruit_MPR121();


#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif


int max_touch = 0; //highest value seen so far
uint32_t current_color = 0;
int blinking = 0;
void setup() {
  if (DEBUG)
    Serial.begin(9600);

  pinMode(11,OUTPUT);
  digitalWrite(11,HIGH); //power for MPR121
  
//  while (!Serial) { delay(10); }
 
  pixels.begin();
  pixels.setBrightness(255);
  display.begin(THINKINK_MONO);

  if (!cap.begin(0x5A)) { 
    display.clearBuffer();
    display.setTextSize(3);
    display.setCursor((display.width() - 180)/2, (display.height() - 24)/2);
    display.setTextColor(EPD_BLACK);
    display.print("NO SENSOR");
    display.display();
    while (1) {};
  }

  //clear LEDS
  display.clearBuffer();
  display.display();


}

void loop() {

  uint16_t currtouched = cap.touched();
  for (uint8_t i=0; i<12; i++) {
    if (currtouched & _BV(i))  {
  
      int pos = 12 - i;
      if (pos > max_touch) {
        //Serial.println("New pos is ");
        if (pos >= 9)
          current_color = pixels.Color(0,255,0);
        else if (pos >= 5)
          current_color = pixels.Color(255,255,0);
        else 
          current_color = pixels.Color(255,0,0);            
      max_touch = pos;

       if (pos == 4 || pos == 8 || pos == 12)
         blinking = 1;
       else
          blinking = 0;
      
      //slow timing on epaper write, duplicate here to get an immediate response
      for (int i=0;i<8;i++)
          pixels.setPixelColor(i,current_color);
      pixels.show();
      
      display.clearBuffer();
      display.setTextSize(3);
      display.setCursor((display.width() - 180)/2, (display.height() - 24)/2);
      display.setTextColor(EPD_BLACK);
      display.print(String(10 * max_touch) +  " degrees");
      display.display();
  
      if (DEBUG)
        Serial.print("Reached position " + String(max_touch));
      }
    }
  }

  
  unsigned long t = millis();
  if (abs(int(t % 500)) > 250 && blinking == 1) 
    for (int i=0;i<8;i++)
        pixels.setPixelColor(i,0);
  else {
    for (int i=0;i<8;i++)
        pixels.setPixelColor(i,current_color);
  }  
  pixels.show();

}
