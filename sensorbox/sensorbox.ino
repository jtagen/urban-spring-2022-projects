//https://www.adafruit.com/product/178
//https://www.adafruit.com/product/1075

#include <Adafruit_NeoPixel.h>

// Interaction settings
const int LED_BRIGHTNESS = 50;  // 0-255, for neopixels only

const float FORCE_THRESHOLD = 400.0;       // percentage of max a person must hold
const int FORCE_TIME = 5000;      // time in seconds they must hold it for, milliseconds
const float FORCE_MAX = 500.0;

const int RING_START = 9; //choose where in rotation the "first" LED will light up

const int STRIP_TIME = 5000;      // minimum time to complete slide strip sensor
const float STRIP_BACKSLIDE = 50.0; // How far they can move "left" before an error. Note: noisy input

//note: max/min values are reversed -> large values at the left
const float STRIP_MAX = 640.0;
const float STRIP_MIN = 340.0;

const int BUTTON_TIME = 5000;      //Time they must hold the push buttons to activate, milliseconds

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
int state = "none";
unsigned long start_time;
int strip_max;

Adafruit_NeoPixel led_strip(20, LED_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel led_ring(12, LED_RING, NEO_GRB + NEO_KHZ800);

int RingConvert(int i) { //handle rotation on installation
  return (i + RING_START) % 12; //12 LEDs
}

void ws_blink(Adafruit_NeoPixel * pixel, uint32_t color) {
  for (int i=0;i<3;i++) {
    for (int j=0;j<20;j++)
      pixel->setPixelColor(j,color);
    pixel->show();
    delay(500);
    pixel->clear();
    pixel->show();
    delay(500);
  }
  pixel->clear();
  pixel->show();
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
  led_ring.begin(); 
  led_ring.setBrightness(LED_BRIGHTNESS);
  led_strip.setBrightness(LED_BRIGHTNESS);
  led_ring.clear();
  led_ring.show();
  
  led_strip.begin(); 
  led_strip.clear();
  led_strip.show();

 if (digitalRead(SW_LEFT) && digitalRead(SW_RIGHT))
  MODE_TEST = 1;  
    
//  if (MODE_TEST) {
    Serial.begin(115200);
    Serial.println("Starting test mode");
//  }

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
  
  //small delay and read seems to clear up initial false readings
  analogRead(SENSOR_FORCE);
  analogRead(SENSOR_STRIP);
  delay(100);

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
    int strip_value = analogRead(SENSOR_STRIP);
    led_strip.clear(); // Set all pixel colors to 'off'
    float strip_percent = (STRIP_MIN - strip_value) / STRIP_MAX ; //invert & scale
    int strip_pixels = round(20.0 * strip_percent);
    led_strip.setPixelColor(strip_pixels, led_strip.Color(255,255,255));
    led_strip.show();  

    int force_value = analogRead(SENSOR_FORCE);
    led_ring.clear();
    if (force_value > 2) {
      float ring_percent = min(100.00,force_value /  FORCE_MAX) ;
      int ring_pixels = RingConvert(round(12.0 * ring_percent));
      led_ring.setPixelColor(ring_pixels, led_ring.Color(255,255,255));
    }
    led_ring.show();  
    delay(50);
    return;
  } 

  int force_value = analogRead(SENSOR_FORCE);
  int strip_value = analogRead(SENSOR_STRIP);
  int leftswitch_pressed = digitalRead(SW_LEFT);
  int rightswitch_pressed = digitalRead(SW_RIGHT);
  
  
  if (state == "none" && leftswitch_pressed) {
    //started pressing switch
    state = "leftswitch";
    analogWrite(LED_LEFT,LED_FORWARD_V);
    start_time = millis();
  }

  if (state == "leftswitch" && !leftswitch_pressed) {
    //failed to hold switch
    analogWrite(LED_LEFT,0);
    state = "none";
  }

  if (state == "leftswitch" && (millis() - start_time) > BUTTON_TIME) {
    //flash green 3 times
    for (int i=0;i<3;i++) {
      analogWrite(LED_LEFT,0);
      delay(500);      
      analogWrite(LED_LEFT,LED_FORWARD_V);
      delay(500);      
    }
    analogWrite(LED_LEFT,0);
    while (digitalRead(SW_LEFT)) {   }    //wait for switch release
    state = "none";

  }

  if (state == "none" && rightswitch_pressed) {
    //started pressing switch
    state = "rightswitch";
    analogWrite(LED_RIGHT,LED_FORWARD_V);
    start_time = millis();
  }

  if (state == "rightswitch" && !rightswitch_pressed) {
    //failed to hold switch
    analogWrite(LED_RIGHT,0);
    state = "none";
  }

  if (state == "rightswitch" && (millis() - start_time) > BUTTON_TIME) {
    //flash green 3 times
    for (int i=0;i<3;i++) {
      analogWrite(LED_RIGHT,0);
      delay(500);      
      analogWrite(LED_RIGHT,LED_FORWARD_V);
      delay(500);      
    }
    analogWrite(LED_RIGHT,0);
    while (digitalRead(SW_RIGHT)) {   }    //wait for switch release
    state = "none";

  }

  if (state == "none" && force_value > 5) {
    Serial.print("Force value is ");
    Serial.println(force_value);
    Serial.println("Entering state force_climb");
    state = "force_climb";
  }

  if (state == "force_climb") {
    float pressure_percent = force_value / FORCE_THRESHOLD;
    int led_num = pressure_percent * 12;
    led_ring.clear();
    for (int i=0;i<led_num;i++)
      led_ring.setPixelColor(RingConvert(i),led_ring.Color(255,255,0));
    led_ring.show();
  }
  
  if (state == "force_climb" && force_value > FORCE_THRESHOLD) {
    Serial.println("Entering state force_maintain");
    //TODO: show a countdown with ring LEDs to indicate hold time?
    for (int i=0;i<12;i++)
      led_ring.setPixelColor(RingConvert(i),led_ring.Color(0,255,0));
    led_ring.show();
    state = "force_maintain";
    start_time = millis();    
  }
  
  if ((state == "force_maintain" && force_value < FORCE_THRESHOLD) || (state == "force_climb" && force_value < 5)) {
    Serial.println("Leaving state force_maintain due to low pressure");
    //failed to maintain pressure
    state = "none";
    //blink red  
    ws_blink(&led_ring,led_ring.Color(255,0,0));
    while (analogRead(SENSOR_FORCE)) { }  // wait for release
    state = "none";
  }

  if (state == "force_maintain" && (millis() - start_time) > FORCE_TIME) {
    Serial.println("Leaving state force_maintain due to time");
    ws_blink(&led_ring,led_ring.Color(0,255,0));
    while (analogRead(SENSOR_FORCE)) { }  // wait for release
    state = "none";
   }

  if (state == "none" && strip_value >= STRIP_MAX) {
    state = "strip_move";
    strip_max = strip_value;
    start_time = millis();
  }

  if (state == "strip_move") {
    strip_max = min(strip_max, strip_value); //max as in how far we've gone, min value. argh!
    //set LEDs;
    float strip_percent = (STRIP_MAX - strip_value) / STRIP_MIN;
    int led_count = strip_percent * 20;
    for (int i=0;i<led_count;i++)
      led_strip.setPixelColor(i,led_strip.Color(0,255,0));    
    led_strip.show();
  }
  
/*
 // Disable backsliding detection. Was failing during demo!!
 
  Serial.print("Backslide val: ");
  Serial.println(strip_value-strip_max);
  if (state == "strip_move" && (strip_value-strip_max) > STRIP_BACKSLIDE) {
    ws_blink(&led_strip,led_strip.Color(255,0,0));
    state = "none";
  }
*/

  if (state == "strip_move" && strip_value == 0) {
    //lifted finger
    state = "none";
    ws_blink(&led_strip,led_strip.Color(255,0,0));  
  }
  

/*
  if (state == "strip_move" && (millis() - start_time) > STRIP_TIME) {
    ws_blink(&led_strip,led_strip.Color(255,0,0));  
    while (analogRead(SENSOR_STRIP)) { }
    state = "none";
  }
  */

  if (state == "strip_move" && strip_value <= STRIP_MIN && strip_value > (STRIP_MIN-50) ) {
    Serial.println(strip_value);
    //success
    Serial.println("Success");
    ws_blink(&led_strip,led_strip.Color(0,255,0));  
    while (analogRead(SENSOR_STRIP)) { }
    state = "none";
  }
}
