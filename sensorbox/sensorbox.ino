//https://www.adafruit.com/product/178
//https://www.adafruit.com/product/1075

#include <Adafruit_NeoPixel.h>


// Interaction settings
const int LED_BRIGHTNESS = 50;  // 0-255, for neopixels only

const float FORCE_THRESHOLD = 80.0;       // percentage of max a person must hold
const int FORCE_TIME = 10;      // time in seconds they must hold it for
const float FORCE_MAX = 500.0;

const int RING_START = 9; //choose where in rotation the "first" LED will light up

const int STRIP_TIME = 10;      // minimum time to complete slide strip sensor
const float STRIP_BACKSLIDE = 10.0; // How far they can move "left" before an error. Note: noisy input
const float STRIP_MIN = 640.0;
const float STRIP_MAX = 340.0;

const int BUTTON_TIME = 5;      //Time they must hold the push buttons to activate



// HARDWARE SETTINGS
const int LED_LEFT = 0;
const int LED_RIGHT = 1;
const int LED_FORWARD_V = 150; // careful, no resistors
const int SW_LEFT = 8;
const int SW_LEFT_PWR = 10;
const int SW_RIGHT = 2;
const int SW_RIGHT_PWR = 5;
const int SENSOR_STRIP = A3;
const int SENSOR_STRIP_PWR = 6;
const int SENSOR_FORCE = A2;
const int SENSOR_FORCE_PWR = A0;
const int LED_STRIP = 4;
const int LED_RING = 12;






int MODE_TEST = 0;

Adafruit_NeoPixel led_strip(20, LED_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel led_ring(12, LED_RING, NEO_GRB + NEO_KHZ800);

int RingConvert(int i) { //handle rotation on installation
  return (i + RING_START) % 12; //12 LEDs
}


void setup() {
  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);

  
  pinMode(SW_LEFT, INPUT);
  pinMode(SW_LEFT_PWR, OUTPUT);
  digitalWrite(SW_LEFT_PWR,HIGH);
  
  pinMode(SW_RIGHT, INPUT);
  pinMode(SW_RIGHT_PWR,OUTPUT);
  digitalWrite(SW_RIGHT_PWR, HIGH);


  pinMode(SENSOR_STRIP, INPUT);
  pinMode(SENSOR_STRIP_PWR, OUTPUT);
  digitalWrite(SENSOR_STRIP_PWR, 1);

  pinMode(SENSOR_FORCE, INPUT);
  pinMode(SENSOR_FORCE_PWR, OUTPUT);
  digitalWrite(SENSOR_FORCE_PWR, 1);
    
  //clear all neopixels
  led_ring.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  led_ring.setBrightness(LED_BRIGHTNESS);
  led_strip.setBrightness(LED_BRIGHTNESS);
  led_ring.clear();
  led_ring.show();
  

  led_strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  led_strip.clear();
  led_strip.show();




 if (digitalRead(SW_LEFT) && digitalRead(SW_RIGHT))
  MODE_TEST = 1;  
    

  if (MODE_TEST) {
    Serial.begin(115200);
    Serial.println("Starting test mode");
  }


  //loading test setup
  analogWrite(LED_LEFT,LED_FORWARD_V);
  analogWrite(LED_RIGHT, LED_FORWARD_V);
  for (int i=0;i<20;i++) {
    led_ring.setPixelColor(i, led_ring.Color(255, 0,0));  
    led_strip.setPixelColor(i,led_strip.Color(255,0,0));
    led_ring.show();
    led_strip.show();
    delay(20);
  }
  analogWrite(LED_LEFT,0);
  analogWrite(LED_RIGHT, 0);

  for (int i=0;i<20;i++) {
    led_ring.setPixelColor(i, led_ring.Color(0, 255,0));  
    led_strip.setPixelColor(i,led_strip.Color(0,255,0));
    led_ring.show();
    led_strip.show();
    delay(20);
  }

  analogWrite(LED_LEFT,LED_FORWARD_V);
  analogWrite(LED_RIGHT, LED_FORWARD_V);
  
  for (int i=0;i<20;i++) {
    led_ring.setPixelColor(i, led_ring.Color(0, 0,255));  
    led_strip.setPixelColor(i,led_strip.Color(0,0,255));
    led_ring.show();
    led_strip.show();
    delay(50);
  }

  analogWrite(LED_LEFT,0);
  analogWrite(LED_RIGHT, 0);

  led_ring.clear();
  led_strip.clear();
  led_ring.show();
  led_strip.show();
  

}





void loop() {

  if (MODE_TEST) {

    Serial.print("Left Button: ");
    Serial.println(digitalRead(SW_LEFT));
    Serial.print("Right Button: ");
    Serial.println(digitalRead(SW_RIGHT));
    Serial.print("Force Sensor: ");
    Serial.println(analogRead(SENSOR_FORCE));
    Serial.print("Strip Sensor: ");
    Serial.println(analogRead(SENSOR_STRIP));
    Serial.println("----------------------------------------");
  
    
    if (digitalRead(SW_LEFT))
      analogWrite(LED_LEFT, LED_FORWARD_V);
    else
      analogWrite(LED_LEFT,0);
  
    
    if (digitalRead(SW_RIGHT))
      analogWrite(LED_RIGHT, LED_FORWARD_V);
    else
      analogWrite(LED_RIGHT, 0);

    
    
    
    delay(50);
  
    int strip_value = analogRead(SENSOR_STRIP);
  
    led_strip.clear(); // Set all pixel colors to 'off'
  
    float strip_percent = (STRIP_MIN - strip_value) / STRIP_MAX ; //invert & scale
  
  
    int pixel_to_show = round(20.0 * strip_percent);
    led_strip.setPixelColor(pixel_to_show, led_strip.Color(255,255,255));
    led_strip.show();  

    int force_value = analogRead(SENSOR_FORCE);
    led_ring.clear();
    if (force_value > 2) {
      float ring_percent = min(100.00,force_value /  FORCE_MAX) ;
      int value_to_show = round(12.0 * ring_percent);

      Serial.print("Pre-convert: ");
      Serial.println(value_to_show);
      value_to_show = RingConvert(value_to_show);

      Serial.print("Post-convert: ");
      Serial.println(value_to_show);

      led_ring.setPixelColor(value_to_show, led_ring.Color(255,255,255));
    }
    led_ring.show();  
    







    return;
  }









return;

/*
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
*/
}
