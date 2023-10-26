#include <Adafruit_NeoPixel.h>

#define LED_PIN 7
#define RPhase 2
#define LED_COUNT 57
#define ZERO_ERROR 0.8
#define MIN_FREQ   1.1
#define MAX_FREQ   25

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

float frequency = 0;
unsigned long startTime = 0;
unsigned long endTime = 0;
bool risingEdge = false;
bool measureFrequency = true;

void setup() {
  pinMode(RPhase, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RPhase), handleInterrupt, CHANGE);
  strip.begin();       
  strip.show();
  strip.setBrightness(255);
}

void loop() {
  measureFrequency = true;
  if (frequency >= MIN_FREQ && frequency <= MAX_FREQ) {
    float timePeriod = (1 / frequency) * 1000;
    for(int i = 0; i < LED_COUNT; i++){
      northSouthChasing(i, timePeriod / LED_COUNT);
    }
    delay(timePeriod / (LED_COUNT * 4)); 
    delay(timePeriod / (LED_COUNT * 4));
  } 
  else if(frequency>25.0 && frequency<50.0){
    float timePeriod = (1 / 38) * 1000;
    for(int i = 0; i < LED_COUNT; i++){
      northSouthChasing(i, timePeriod / LED_COUNT);
    }
  }
  else{
    strip.clear();
    strip.setPixelColor(LED_COUNT / 2, strip.Color(255, 255, 0));
    strip.show();
    delay(500);
    strip.clear();
    strip.show();
    delay(500);
  }
}

void handleInterrupt() {
  delayMicroseconds(100);
  if (measureFrequency) {
    int sensorValue = digitalRead(RPhase);
    if (sensorValue == HIGH && !risingEdge) {
      startTime = micros();
      risingEdge = true;
    } else if (sensorValue == LOW && risingEdge) {
      endTime = micros();
      risingEdge = false;
      frequency = (1.0 / ((endTime - startTime) * 1e-6)) / 2 + ZERO_ERROR;
    }
  }
}

void northSouthChasing(int index, float wait) {
    int redIndex = index;
    int blueIndex = (index + (LED_COUNT / 2)) % LED_COUNT;
    strip.clear(); 
    strip.setPixelColor(redIndex, strip.Color(255, 0,0));
    strip.setPixelColor(blueIndex, strip.Color(0, 0, 255));
    strip.show();
    delay(wait);
}