#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <PulseSensorPlayground.h>
const int OUTPUT_TYPE = SERIAL_PLOTTER;
const int PULSE_INPUT = A0;
const int PULSE_BLINK = LED_BUILTIN;
const int PULSE_FADE = 5;




const int THRESHOLD = 250;   // Adjust this number to avoid noise when idle
const int FALL_ACCEL_MSS = 14;
const int HARD_FALL_ACCEL_MSS = 20;
const int SAMPLE_WINDOW_SECONDS = 1;
const int SAMPLE_LOOKBACK_SECONDS = 10; // Must be a multiple of above


Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_AHTX0 aht;
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
PulseSensorPlayground pulseSensor;

// Create a ring buffer to hold samples
int temperature_history[(SAMPLE_LOOKBACK_SECONDS) / SAMPLE_WINDOW_SECONDS];
int humidity_history[(SAMPLE_LOOKBACK_SECONDS) / SAMPLE_WINDOW_SECONDS];
int pulse_history[(SAMPLE_LOOKBACK_SECONDS) / SAMPLE_WINDOW_SECONDS];
int history_pos = 0; //starting index in ring buffer

unsigned long last_sample = millis();
unsigned long last_redraw = millis();

int temperature_average = 0;
int humidity_average = 0;
int pulse_average = 0;
int average_position = 0;

int temperature_last = 0;
int humidity_last = 0;
int pulse_last = 0;
String position_last = "Unknown";

float p = 3.1415926;

void setup(void) {
  Serial.begin(9600);
  Serial.print(F("Hello! Debug interface for headband_fall_down"));

  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize TFT
  tft.init(135, 240); // Init ST7789 240x135
  tft.setRotation(3);
  ShowLoading();


  Serial.println(F("TFT Initialized"));

  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT initialized");
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  if (!accel.begin()) {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while (1)
      ;
  }
  accel.setRange(LSM303_RANGE_4G);
  accel.setMode(LSM303_MODE_HIGH_RESOLUTION);
  //accel.setMode(LSM303_MODE_NORMAL);
   pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.setThreshold(THRESHOLD);

  Serial.println("Accelerometer initialized");

  sensors_event_t event;
  accel.getEvent(&event);
  Serial.print("X: ");
  Serial.print(event.acceleration.x);
  Serial.print("  ");
  Serial.print("Y: ");
  Serial.print(event.acceleration.y);
  Serial.print("  ");
  Serial.print("Z: ");
  Serial.print(event.acceleration.z);
  Serial.print("  ");
  Serial.println("m/s^2");



  if (!pulseSensor.begin()) {
      /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no pulse sensor detected ... Check your wiring!");
    while (1)
      ;
  }

}

void loop() {


  // Store a single in our "average for the window" variable
  pulse_last = pulseSensor.getBeatsPerMinute();
  pulse_average += pulse_last;
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  temperature_last = temp.temperature;
  humidity_last = humidity.relative_humidity;
  temperature_average += temperature_last;
  humidity_average += humidity_last;
  average_position++;
  Serial.println("Sampling....");
  Serial.println(pulse_last);
  Serial.println(temperature_last);
  Serial.println(humidity_last);

  sensors_event_t accel_reading;
  accel.getEvent(&accel_reading);
  int overall_acceleration = sqrt(pow(accel_reading.acceleration.x,2) + pow(accel_reading.acceleration.y,2) + pow(accel_reading.acceleration.z,2));

  Serial.print("Accel: ");
  Serial.print(accel_reading.acceleration.x);
  Serial.print(" ");
  Serial.print(accel_reading.acceleration.y);
  Serial.print(" ");
  Serial.println(accel_reading.acceleration.z);
  
  

  if ((millis() - last_redraw) / 1000 > 1) {
    // Limit updates to once per second
    last_redraw = millis();
    ShowStatus();

  }


  // SAMPLE_WINDOW_SECONDS has gone by, store it
  if ((millis() - last_sample) / 1000 > SAMPLE_WINDOW_SECONDS) {
    Serial.println("Storing sample in ring");
    temperature_history[history_pos] = temperature_average / average_position;
    humidity_history[history_pos] = humidity_average / average_position;
    pulse_history[history_pos] = pulse_average / average_position;
    history_pos = (history_pos + 1) % (SAMPLE_LOOKBACK_SECONDS) / SAMPLE_WINDOW_SECONDS; 

    temperature_average = 0;
    humidity_average = 0;
    pulse_average = 0;
    average_position = 0;


    last_sample = millis();
  }



  // Check if we've experienced a hard fall
  if (overall_acceleration > HARD_FALL_ACCEL_MSS) {
    ShowFall("Hard");
  }

  // Check if we've experienced a non-hard fall

  if (overall_acceleration > FALL_ACCEL_MSS) {
    ShowFall("");
  }

  // See if we changed position

  if (accel_reading.acceleration.y < -8) {
    position_last = "Standing";
  }

  if (accel_reading.acceleration.z > 8) {
    position_last = "Lying";
  }


}

void ShowLoading() {
  drawtext("Measuring....", ST77XX_WHITE, 3);
}
void ShowStatus() {

  //Not instantaneous, rather than average over last 0-30 seconds
  int pulse = pulse_average / average_position;
  float temp = ((temperature_average / average_position) * 9/5) + 32;
  float humidity = humidity_average / average_position;
  float temperature_display = (temperature_last * 9/5) + 32;
//  String text = "Heart Rate:\n.  " + String(pulse) + "\nTemperature:\n.  " + String(temp) + "F\nHumidity:\n.  " + String(humidity) + "\nPosition:\n.  " + position;
  String text = "HR: " + String(pulse_last) + "\nTemp: " + String(temperature_display) + "F\nRH: " + String(humidity_last) + "\nPos: " + position_last;

  drawtext(text,ST77XX_WHITE,3);
}


void ShowFall(String type) {
  String text = type + "  FALL\n";
  drawtext(text, ST77XX_RED, 4);
  tft.setCursor(0, 50);
  
  float temperature_display = (temperature_history[history_pos-1] * 9/5) + 32; 
  String buf = "HR: " + String(pulse_history[history_pos-1]) + "\nTemp: " + String(temperature_display) + "F\nRH: " + String(humidity_history[history_pos-1]) + "\nPos: " + position_last;

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(buf);

  
 
 
 for (int i=0;i<60*4;i++) {
     tft.invertDisplay(true);
    delay(500);
    tft.invertDisplay(false);
    delay(500);

  }
}

void drawtext(String text, uint16_t color, int size) {
  char buf[500]; //convert to character array
  text.toCharArray(buf, 500);


  tft.setCursor(0, 0);
  tft.setTextSize(size);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(buf);
}

