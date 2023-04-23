
#include <Adafruit_NeoPixel.h>
#include <Wire.h> 


Adafruit_NeoPixel pixels[6];


void requestEvent() 
{
  Serial.println("Received data");

 while (0 < Wire.available()) {
    byte x = Wire.read();
    Serial.print(x);
    if (x > 90) {
      int offset = x - 65;
      Serial.print("Turning on " + offset);
      pixels[offset].setPixelColor(0, pixels[offset].Color(255, 0, 0));
    } else {
      int offset = x - 97;
      Serial.print("Turning off " + offset);
      pixels[offset].setPixelColor(0, pixels[offset].Color(0, 0, 0));
    }
  }
  
}


void setup() {

  Serial.begin(9600);

  pixels[0] = Adafruit_NeoPixel(1, A0, NEO_GRB + NEO_KHZ800);
  pixels[1] = Adafruit_NeoPixel(1, A1, NEO_GRB + NEO_KHZ800);
  pixels[2] = Adafruit_NeoPixel(1, A2, NEO_GRB + NEO_KHZ800);
  pixels[3] = Adafruit_NeoPixel(1, A3, NEO_GRB + NEO_KHZ800);
  pixels[4] = Adafruit_NeoPixel(1, 10, NEO_GRB + NEO_KHZ800);
  pixels[5] = Adafruit_NeoPixel(1, 11, NEO_GRB + NEO_KHZ800);

  for (int i=0;i<6;i++) {
    pixels[i].begin();
  }

  Wire.begin(7);                // join i2c bus with address #2 
  Wire.onRequest(requestEvent); // register event 
  Wire.onReceive(requestEvent); // register event 


}

void loop() {
  for (int i=0;i<6;i++) {
    pixels[i].setPixelColor(0, pixels[i].Color(100, 0, 0));
    pixels[i].show();
    delay(500); // Pause before next pass through loop
    pixels[i].setPixelColor(0, pixels[i].Color(0, 0, 0));
    pixels[i].show();
  }

  for (int i=0;i<6;i++) {
    pixels[i].setPixelColor(0, pixels[i].Color(0, 100, 0));
    pixels[i].show();
    delay(500); // Pause before next pass through loop
    pixels[i].setPixelColor(0, pixels[i].Color(0, 0, 0));
    pixels[i].show();
  }

  for (int i=0;i<6;i++) {
    pixels[i].setPixelColor(0, pixels[i].Color(0, 0, 100));
    pixels[i].show();
    delay(500); // Pause before next pass through loop
    pixels[i].setPixelColor(0, pixels[i].Color(0, 0, 0));
    pixels[i].show();
  }



}



