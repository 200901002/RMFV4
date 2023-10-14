#include <Adafruit_NeoPixel.h>

#define LED_PIN    7
#define RPhase 3
#define LED_COUNT 54
#define ZERO_ERROR 0.8

#define MIN_FREQ   1.1
#define MAX_FREQ   50

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
  Serial.begin(9600);
}

void loop() {
  measureFrequency = true;
  if (frequency >= MIN_FREQ && frequency <= MAX_FREQ) {
    Serial.println(frequency);
    float timePeriod = (1 / frequency) * 1000; // Calculate the time period of one cycle in milliseconds
    for(int i = 0; i < LED_COUNT; i++){
      northSouthChasing(i, timePeriod / LED_COUNT); // Call the function to create the chasing effect with each LED having a fraction of the time period
    }
    // delay(timePeriod / (LED_COUNT * 4)); // Flux - voltage delay. This is to account for the phase difference between the voltage and the current in an AC circuit
    // delay(timePeriod / (LED_COUNT * 4)); // Wait for R peak. This is to synchronize the LED animation with the peak of the R wave.
  } 
  else {
    // Invalid frequency, blink the middle LED with yellow color
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
  delayMicroseconds(100); // Debounce the input signal to avoid false triggers
  if (measureFrequency) {
    int sensorValue = digitalRead(RPhase);

    if (sensorValue == HIGH && !risingEdge) {
      startTime = micros(); // Record the start time of a high pulse
      risingEdge = true;
    } else if (sensorValue == LOW && risingEdge) {
      endTime = micros(); // Record the end time of a high pulse
      risingEdge = false;
      frequency = (1.0 / ((endTime - startTime) * 1e-6)) / 2 + ZERO_ERROR;
    }
  }
}

void northSouthChasing(int index, float wait) {
    int redIndex = index; 
    int blueIndex = (index + (LED_COUNT / 2)) % LED_COUNT;
    strip.clear(); 
    strip.setPixelColor(redIndex, strip.Color(255, 0,0)); // Set the main red pixel
    strip.setPixelColor(blueIndex, strip.Color(0, 0, 255)); // Set the main blue pixel
    strip.show();
    delay(wait); 
}
