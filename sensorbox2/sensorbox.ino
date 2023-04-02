//https://www.adafruit.com/product/178
//https://www.adafruit.com/product/1075

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

Adafruit_NeoPixel pixels(20, 6, NEO_GRB + NEO_KHZ800);


void setup() {

  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
  pinMode(8,OUTPUT);
  digitalWrite(8,HIGH); //temporary pull-up w/ 10k 
  pinMode(A0,INPUT);  
  Serial.begin(115200);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();
  pixels.show();
}

void loop() {




  int strip = analogRead(A0);

  if (strip > 700)
    return; //not being touched

    //650 = left
    //350 = right

  pixels.clear(); // Set all pixel colors to 'off'

  float strip_percent = (700.0 - strip) / 350.0 ; //invert & scale


  int pixels_to_show = 20 * strip_percent;
 
  for(int i=0; i<20; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    if (i > pixels_to_show)
      pixels.setPixelColor(i, pixels.Color(0, 0,0));
    else
      pixels.setPixelColor(i, pixels.Color(0, 150,0));
  }
  pixels.show();  


    
   Serial.println(strip);
   Serial.println(strip_percent);
   Serial.println("");
   delay(500);
   return;
     

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for(int i=0; i<20; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(150, 150,150));

    pixels.show();   // Send the updated pixel colors to the hardware.

//    delay(DELAYVAL); // Pause before next pass through loop
  }
}
