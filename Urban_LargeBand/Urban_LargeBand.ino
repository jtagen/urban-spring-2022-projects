
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




struct Message {
  int combo;
  String text;
  int time;
  int size;
  int colors[4];
};


int d15_count = 24;
Message d15_messages[] = {
    {4,"Hello",5,6,{0,0,0,0}}, /* * */
    {3,"Goodbye",5,6,{0,0,0,0}}, /* * */
    {1,"Yes",5,6,{0,255,0,0}}, /* * */
    {0,"No",5,6,{255,0,0,0}}, /* * */
    {6,"My name\nis John",5,5,{0,0,0,0}}, /* * */
    {7,"My phone number is 123-456-7890",5,3,{0,0,0,0}}, /* * */
    {8,"I live in\nProvidence",5,4,{0,0,0,0}},  /* * */
    {10,"I am a junior\nand 21 years old",5,3,{0,0,0,0}},  /* * */
    {64,"Have you\ndone the\nhomework yet?",5,3,{0,0,0,0}},  /* * */
    {74,"When is\nthe homework\ndue?",5,4,{0,0,0,0}},
    {84,"What time\ndoes class\nstart?",5,4,{0,0,0,0}},
    {104,"When is the\nnext exam?",5,4,{0,0,0,0}}, /* * */
    {63,"Can I have a\ngrande iced\nvanilla latte?",5,3,{0,0,0,0}},
    {73,"Can I have a\nplain cheese\npizza?",5,3,{0,0,0,0}},
    {83,"Can I have a\nham and cheese\nhoagie?",5,3,{0,0,0,0}},
    {103,"Can I have\nplain pasta?",5,4,{0,0,0,0}},
    {61,"Can you point\nme to the\n Deans office?",5,3,{0,0,0,0}},
    {71,"Where is the\nYena Center?",5,4,{0,0,0,0}},
    {81,"Which way is\nthe Xavier Academic\nComplex?",5,3,{0,0,0,0}},
    {101,"Where is the \nharborside\ncampus?",5,4,{0,0,0,0}},
    {60,"Can you put\n$20 on\npump 7?",5,4,{0,0,0,0}},
    {70,"Can you point me\nto the nearest\nbathroom",5,3,{0,0,0,0}},
    {80,"What time are\nwe going out?",5,3,{0,0,0,0}},
    {100,"Can you\norder\nthe uber?",5,4,{0,0,0,0}},
};


int d14_count = 24;
Message d14_messages[] = {
  	{4,"Hello",5,6,{0,0,0,0}}, 
    {3,"Goodbye",5,6,{0,0,0,0}}, 
    {1,"Yes",5,6,{0,255,0,0}},
    {0,"No",5,6,{255,0,0,0}},
    {6,"My name\nis Jane",5,5,{0,0,0,0}}, 
    {7,"I am 7\nyears old",5,5,{0,0,0,0}}, 
    {8,"I am a\nperson with\nautism",5,4,{0,0,0,0}},  
    {10,"This is\nhow I\ncommunicate",5,4,{0,0,0,0}}, 

    {64,"Can you help me find my family?",5,3,{0,0,0,0}}, 
    {74,"My moms\nname is\nSarah Smith",5,4,{0,0,0,0}},
    {84,"My brothers\nname is\nJohnny",5,4,{0,0,0,0}},
    {104,"My dads\nname is\nBob Smith",5,4,{0,0,0,0}},

    {63,"Do you want\nto go play\ninside?",5,4,{0,0,0,0}},
    {73,"Do you want\nto go play\noutside?",5,3,{0,0,0,0}},
    {83,"Whats your\nfavorite\ngame?",5,4,{0,0,0,0}},
    {103,"Whats your\ndolls name?\nMine is Molly!",5,3,{0,0,0,0}},

    {61,"Can I go to the bathroom?",5,3,{0,0,0,0}},
    {71,"What special\nis today?",5,4,{0,0,0,0}},
    {81,"What's for\nlunch?",5,4,{0,0,0,0}},
    {101,"I'm on\nbus 107",5,5,{0,0,0,0}},

    {60,"What time\ndoes dance\nstart?",5,4,{0,0,0,0}},
    {70,"Can I\nget water?",5,3,{0,0,0,0}},
    {80,"Can we\ngo over\nthat again?",5,3,{0,0,0,0}},
    {100,"Do we have\npractice \ntomorrow?",5,3,{0,0,0,0}},
};



Message * messages = d15_messages;
int message_count = d15_count;


/*
//Set 1:
4   Hello
3		Goodbye
1		Yes
0		No
6		My name is John
7		My phone number is 123-456-7890
8		I live in Providence
10		I am a junior, and 21 years old
4	6	Have you done the homework yet?
4	7	When is the homwork due?
4	8	What time does class start?
4	10	When is the next exam?
3	6	Can I have a grande iced vanilla latte?
3	7	Can I have a plain cheese pizza?
3	8	Can I have a ham and cheese hoagie?
3	10	Can I have plain pasta?
1	6	Can you point me to the Deans office?
1	7	Where is the Yena Center?
1	8	Which way is the Xavier Acadmic Complex?
1	10	Where is the harborside campus?
0	6	Can you put $20 on pump 7?
0	7	Can you point me to the nearest bathroom?
0	8	What time are we going out?
0	10	Can you order the uber?

Set 2:

4		Hello	
3		Goodbye	
1		Yes	If possible, lights green
0		No	If possible, lights red
6		My name is Jane	
7		I am 7 years old	
8		I am a person with autism	
10		This is how I communicate	
4	6	Can you help me find my family?	
4	7	My moms name is Sarah Smith	
4	8	My brothers name is Johnny	
4	10	My dads name is Bob Smith	
3	6	Do you want to go play inside?	
3	7	Do you want to go play outside?	
3	8	Whats you favorite game?	
3	10	Whats your dolls name? Mine is Molly!	
1	6	Can I go to the bathroom?	
1	7	What special is today?	
1	8	Whats for lunch?	
1	10	I'm on bus 107	
0	6	What time does dance start?	
0	7	Can I get water?	
0	8	Can we go over that again?	
0	10	Do we have practice tomorrow?	



*/




/*
old messages

String message_text[13] = {"","EMERGENCY!\nPlease help","Let me\ntext you","Can you please\ntalk slower?","123-456-7891","","Thank\nyou","","Hi! My name\nis Jo!","","No","Yes",""};
int message_times[13] = {0,-1,5,5,5,5,5,5,5,5,5,5,5}; //seconds
int message_size[13] = {0,4,5,3,4,5,5,7,4,0,11,9,0}; //weird internal font size
int message_colors[13][4] = {{0,0,0,0},{255,0,0,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{255,0,0,0},{0,255,0,0},{0,0,0,0}}; //R,G,B,emergency


1 = When you press 1, Yes appears - 11
2 = When you press 2, No appears - 10
3 = When you press 3, Hi! My name is Jo! appears  - 8
4 = When you press 4, Thank you appears - 6
5 = When you press 5, 123-456-7891 (phone number) appear - 4
6 = Can you please talk slower? - 3
7 = Let me text you appears  - 2
8 = EMERGENCY! Please help appears - 1
*/


void flash(String m) {
  //display current message
  display.clearBuffer();
  display.setTextSize(5);
  display.setTextColor(EPD_BLACK);
  display.setCursor(5, 5);
  display.print(m);
  display.display();
  delay(2000);    
  display.clearBuffer();
  display.display();


}

void setup() {
  Serial.begin(9600);
  Serial.println("Serial");
  //blank the screen to begin
  display.begin(THINKINK_GRAYSCALE4);
  display.clearBuffer();
  display.display();
  pinMode(BUTTON_A,INPUT_PULLUP);
  pinMode(BUTTON_B,INPUT_PULLUP);


   if (!cap.begin(0x5A)) { 
    //Note: magtag goes into an error mode,
    //flashing LEDs when this happens

    display.clearBuffer();
    display.setTextSize(2);
    display.setTextColor(EPD_BLACK);
    display.setCursor(5, 5);
    display.print("NO CAP DEVICE FOUND");
    display.display();


     while (1) {delay(50);};
  }

  //disable speaker, conserve power
  pinMode(SPEAKER,OUTPUT);
  pinMode(SPEAKER_SHUTDOWN, OUTPUT);
  digitalWrite(SPEAKER_SHUTDOWN, LOW); //off



  //disable LEDs
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH); // off

}

void loop() {


  if (!digitalRead(BUTTON_A)) {
    messages = d15_messages;
    message_count = d15_count;
    Serial.println("Switching to college profile");
    flash("College");
  }

  if (!digitalRead(BUTTON_B)) {
    messages = d14_messages;
    message_count = d14_count;
    Serial.println("Switching to pediatric profile");
    flash("Pediatric");
  }



  uint16_t currtouched = cap.touched();
  uint8_t touch_combo = 255;
  int found_message = -1;

  if (currtouched > 0) {
    delay(100); //wait 100 ms then re-measure to account for people not hitting multiple elements at the same time
    currtouched = cap.touched();
  }

  for (uint8_t i=0; i<12; i++) {
    if ((currtouched & _BV(i))) {
      if (touch_combo == 255) {
        touch_combo = i;
      } else {
        touch_combo = max(i,touch_combo) * 10 + min(i,touch_combo);
      }
    }
  }
  
    //figure out which array element we should display
  for (int j=0;j<message_count;j++) {
    if (messages[j].combo == touch_combo) {
      found_message = j;
      break;
    }
  }
  if (found_message != -1 && found_message != message_displayed) {
    message_displayed = found_message;  
    message_display_time = millis();      


    if (messages[found_message].colors[0] || messages[found_message].colors[1] || messages[found_message].colors[2]) {
      digitalWrite(NEOPIXEL_POWER, LOW); // on
      pixels.begin();
      pixels.setBrightness(50);
      uint32_t color = pixels.Color(messages[found_message].colors[0], messages[found_message].colors[1], messages[found_message].colors[2]);
      pixels.fill(color);
      pixels.show();
    } else {


      //set colors to none
      pixels.begin();
      pixels.setBrightness(50);
      pixels.fill(0x000000);
      pixels.show();
      digitalWrite(NEOPIXEL_POWER, HIGH); // off

    }

    Serial.println("Switching to message " + String(found_message));
    Serial.println("Message text: " + messages[found_message].text);
    Serial.println("Displaying for " + String(messages[message_displayed].time) + " seconds");
    Serial.println("Displaying at " + String(messages[message_displayed].size) + " size");

    //display current message
    display.clearBuffer();
    display.setTextSize(messages[found_message].size);
    display.setTextColor(EPD_BLACK);
    display.setCursor(5, 5);
    display.print(messages[found_message].text);
    display.display();
  


  }

  //check if old message expired
  if (message_displayed != -1 && messages[message_displayed].time != -1 && (message_display_time + messages[message_displayed].time * 1000) <= millis()) {

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
  
  if (message_displayed != -1 && messages[message_displayed].colors[3] == 1) {
    digitalWrite(SPEAKER_SHUTDOWN, HIGH); //speaker on
    int t = millis() - message_display_time;
    if ((t % 500) > 250 && blink_state == 0) { 
      blink_state = 1;
      pixels.begin();
      pixels.setBrightness(50);
      uint32_t color = pixels.Color(messages[message_displayed].colors[0], messages[message_displayed].colors[1], messages[message_displayed].colors[2]);
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

