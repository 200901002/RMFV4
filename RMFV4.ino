#define ZeroError 0.8
int j;

const int EnablePin = 9;
const int decoderSelectPins[] = { 10, 11, 12 };  // A0, A1, A2
const int numPins = 6;
const int burningConstant = 2;           // arbitrarily chosen value
const int numStates = 1 << numPins - 2;  //only 6 leds

//Frequency
const int RphaseIntPin = 2;
float frequency = 0.0;
volatile unsigned long startTime = 0;
volatile unsigned long endTime = 0;
volatile bool risingEdge = false;
volatile bool measureFrequency = false;
const float minFrequency = 0.0;
const float maxFrequency = 50.0;

void setup() {

  pinMode(EnablePin, OUTPUT);
  pinMode(RphaseIntPin, INPUT_PULLUP);
  for (int pin : decoderSelectPins) {
    pinMode(pin, OUTPUT);
  }
  digitalWrite(EnablePin, HIGH);  // HIGH => OUTPUTS DISABLED ; LOW => OUTPUTS ENABLED
  attachInterrupt(digitalPinToInterrupt(RphaseIntPin), handleInterrupt, CHANGE);
  Serial.begin(9600);
}

void loop() {
  // measureFrequency = true;
  // delay(500);
  // measureFrequency = false;
  frequency = 2;
  if (isValidFrequency(frequency)) {
    Serial.print("Frequency: ");
    Serial.println(frequency);
    float timePeriod = (1 / frequency) * 100;  // Very important.
    delay(timePeriod / 4);                     // Flux - voltage delay.
    delay(timePeriod / 4);                     // wait for R peak.
    for (j = 0; j < numPins; j++) {
      digitalWrite(decoderSelectPins[j], bitRead(0, j));
    }

    digitalWrite(EnablePin, HIGH);// OUTPUTS ENABLED
    delay(timePeriod / burningConstant);  // led 1 burns for timePeriod/8 ms (8ms randomly chosen)
    digitalWrite(EnablePin, LOW);// OUTPUTS DISABLED
    
    forwardAndReverseSequence(timePeriod , 1);

  }
}

bool isValidFrequency(float freq) {
  return freq >= minFrequency && freq <= maxFrequency;
}

void forwardAndReverseSequence(int delayTime, bool direction) {
  int start = direction ? 0 : numPins - 1;
  int end = direction ? numPins : -1;
  int step = direction ? 1 : -1;
  unsigned long startMillis = millis();
  while (millis() - startMillis < 5000) {
    for (int i = start; i != end; i += step) {

      delay(delayTime / 24);
      for (j = 0; j < numPins; j++) {
        digitalWrite(decoderSelectPins[j], bitRead(i, j));
      }                                    // led 'i' is fired
      digitalWrite(EnablePin, LOW);        // OUTPUTS ENABLED
      delay(delayTime / burningConstant);  // led 'i' burns for timePeriod/8 ms
      digitalWrite(EnablePin, HIGH);       // OUTPUTS DISABLED
    }
  }
}

void handleInterrupt() {
  delayMicroseconds(100);  // Give some time for the processor to handle interrupts.
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