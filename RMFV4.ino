#include <Adafruit_NeoPixel.h>
#define ZeroError 0.8
#define buringConstant 1

#define LED_PIN    7
#define LED_COUNT 10

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int j;


//Frequency
const int RphaseIntPin = 2;
float frequency = 15;
volatile unsigned long startTime = 0;
volatile unsigned long endTime = 0;
volatile bool risingEdge = false;
volatile bool measureFrequency = false;
const float minFrequency = 0.0;
const float maxFrequency = 50.0;

void setup() {
  pinMode(RphaseIntPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RphaseIntPin), handleInterrupt, CHANGE);
  strip.begin();       
  strip.show();
  strip.setBrightness(150);
  Serial.begin(9600);
}

void loop() {
  frequency = 55;
  // measureFrequency = true;
  // delay(600);
  calcuatingFreqPattern(5);
  // measureFrequency = false;
  
  if (isValidFrequency(frequency)) {
    Serial.print("Frequency: ");
    Serial.println(frequency);
    float timePeriod = (1 / frequency) * 1000;  // Very important.
    NorthSouthChasing(timePeriod/buringConstant);
    // delay(timePeriod / 4);                     // Flux - voltage delay.
    // delay(timePeriod / 4);                     // wait for R peak.  
  }
}

bool isValidFrequency(float freq) {
  return freq >= minFrequency && freq <= maxFrequency;
}       
void handleInterrupt() {
  delay(100);
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
    int redIndex = i; // Offset for the red light
    int blueIndex = (i + 5) % strip.numPixels(); // Offset for the blue light with a fixed offset of 4
    strip.clear();
    strip.setPixelColor(redIndex, strip.Color(255, 1,5)); // Set the main red pixel
    strip.setPixelColor(blueIndex, strip.Color(5, 1,255)); // Set the main blue pixel
    strip.show();
    delay(wait);
  }
}
void calcuatingFreqPattern(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    strip.rainbow(firstPixelHue);
    strip.show(); 
    delay(wait);
  }
}