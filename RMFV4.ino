#include <Adafruit_NeoPixel.h>
#define ZeroError 0.8
#define buringConstant 1

#define LED_PIN    7
#define LED_COUNT 54

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int j;

const int RphaseIntPin = 3;
float frequency = 0;
volatile unsigned long startTime = 0;
volatile unsigned long endTime = 0;
volatile bool risingEdge = false;
volatile bool measureFrequency = true;
const float minFrequency = 1.1;
const float maxFrequency = 50;

void setup() {
  pinMode(RphaseIntPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RphaseIntPin), handleInterrupt, CHANGE);
  strip.begin();       
  strip.show();
  strip.setBrightness(255);
  Serial.begin(9600);
}

void loop() {

  // measureFrequency = false;
  // delay(100);
  measureFrequency = true; 
  if (isValidFrequency(frequency)) {
    Serial.println(frequency);
    float timePeriod = (1 / frequency) * 1000;
    for(int i=0;i<10;i++){
    NorthSouthChasing(timePeriod/LED_COUNT);

    }
    delay(timePeriod / (LED_COUNT*4));// Flux - voltage delay.
    delay(timePeriod / (LED_COUNT*4));// wait for R peak.  
  }
   }


bool isValidFrequency(float freq) {
  return freq >= minFrequency && freq <= maxFrequency;
}  


void handleInterrupt() {
  delayMicroseconds(100);
  if (measureFrequency) {
    int sensorValue = digitalRead(RphaseIntPin);

    if (sensorValue == HIGH && !risingEdge) {
      startTime = micros();
      risingEdge = true;
    } else if (sensorValue == LOW && risingEdge) {
      endTime = micros();
      risingEdge = false;
      frequency = 1.0 / ((endTime - startTime) * 1e-6);
      frequency = frequency / 2 + ZeroError;
    }
  }
}


void NorthSouthChasing(float wait) {
  for (int i = 0; i < strip.numPixels(); i++) {
    int redIndex = i; 
    int blueIndex = (i + (LED_COUNT/2)) % strip.numPixels();
    strip.clear();
    strip.setPixelColor(redIndex, strip.Color(255, 0,0)); // Set the main red pixel
    strip.setPixelColor(blueIndex, strip.Color(0, 0, 255)); // Set the main blue pixel
    strip.show();
    delay(wait);
  
  }
}