
//reference: https://learn.adafruit.com/adafruit-magtag/arduino-basics

#include "Adafruit_ThinkInk.h"
#include <Wire.h>
#include "Adafruit_MPR121.h"
#include "Adafruit_NeoPixel.h"



#define EPD_DC      7 
#define EPD_CS      8
#define EPD_BUSY    -1
#define SRAM_CS     -1 
#define EPD_RESET   6
#define SPEAKER_SHUTDOWN 16 //GPIO16
#define SPEAKER A0 //GPIO17


#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif


ThinkInk_290_Grayscale4_T5 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(4, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Adafruit_MPR121 cap = Adafruit_MPR121();
int blink_state = 0;

int message_displayed = -1;
unsigned long message_display_time;



//TODO: deep sleep
//TODO: battery monitor, display message and quit if low

//messages are indexed by MPR121 pin number

String message_text[13] = {"","EMERGENCY!\nPlease help","Let me\ntext you","Can you please\ntalk slower?","123-456-7891","","Thank\nyou","","Hi! My name\nis Jo!","","No","Yes",""};
int message_times[13] = {0,-1,5,5,5,5,5,5,5,5,5,5,5}; //seconds
int message_size[13] = {0,4,5,3,4,5,5,7,4,0,11,9,0}; //weird internal font size
int message_colors[13][4] = {{0,0,0,0},{255,0,0,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{255,0,0,0},{0,255,0,0},{0,0,0,0}}; //R,G,B,emergency

/*
1 = When you press 1, Yes appears - 11
2 = When you press 2, No appears - 10
3 = When you press 3, Hi! My name is Jo! appears  - 8
4 = When you press 4, Thank you appears - 6
5 = When you press 5, 123-456-7891 (phone number) appear - 4
6 = Can you please talk slower? - 3
7 = Let me text you appears  - 2
8 = EMERGENCY! Please help appears - 1
*/



void setup() {

   if (!cap.begin(0x5A)) { 
    //Note: magtag goes into an error mode,
    //flashing LEDs when this happens
     while (1) {};
  }

  //disable speaker, conserve power
  pinMode(SPEAKER,OUTPUT);
  pinMode(SPEAKER_SHUTDOWN, OUTPUT);
  digitalWrite(SPEAKER_SHUTDOWN, LOW); //off



  //disable LEDs
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH); // off
  
  //blank the screen to begin
  display.begin(THINKINK_GRAYSCALE4);
  display.clearBuffer();
  display.display();

}

void loop() {
  uint16_t currtouched = cap.touched();
  
  
  //check for new message
  for (uint8_t i=0; i<12; i++) {
    if ((currtouched & _BV(i)) && message_displayed != i ) {
      message_displayed = i;
      message_display_time = millis();      

      if (message_colors[i][0] || message_colors[i][1] || message_colors[i][2]) {
        digitalWrite(NEOPIXEL_POWER, LOW); // on
        pixels.begin();
        pixels.setBrightness(50);
        uint32_t color = pixels.Color(message_colors[i][0], message_colors[i][1], message_colors[i][2]);
        pixels.fill(color);
        pixels.show();

      }

      //display current message
      display.clearBuffer();
      display.setTextSize(message_size[i]);
      display.setTextColor(EPD_BLACK);
      display.setCursor(5, 5);
      display.print(message_text[i]);
      display.display();

 

    }
  }

  //check if old message expired
  if (message_displayed != -1 && message_times[message_displayed] != -1 && (message_display_time + message_times[message_displayed] * 1000) <= millis()) {

    message_displayed = -1;
    
    //set colors to none
    pixels.begin();
    pixels.setBrightness(50);
    pixels.fill(0x000000);
    pixels.show();
    digitalWrite(NEOPIXEL_POWER, HIGH); // off
    
    //clear e-ink
    display.clearBuffer();
    display.display();

    
    //power-down speaker
    digitalWrite(SPEAKER_SHUTDOWN, LOW);

  }

  
  //emergency beep / flash timing
  
    if (message_displayed != -1 && message_colors[message_displayed][3] == 1) {
    digitalWrite(SPEAKER_SHUTDOWN, HIGH); //speaker on
    int t = millis() - message_display_time;
    if ((t % 500) > 250 && blink_state == 0) { 
      blink_state = 1;
      pixels.begin();
      pixels.setBrightness(50);
      uint32_t color = pixels.Color(message_colors[message_displayed][0], message_colors[message_displayed][1], message_colors[message_displayed][2]);
      pixels.fill(color);
      pixels.show();
    }

    if ((t % 500) < 250 && blink_state == 1) { 
      blink_state = 0;
      pixels.begin();
      pixels.setBrightness(50);
      pixels.fill(0x000000);
      pixels.show();

    }


    digitalWrite(SPEAKER, blink_state * 255 * (millis() %2));

  }


}
