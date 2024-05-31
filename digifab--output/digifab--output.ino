#include <Adafruit_NeoPixel.h>

#define BTN D9
#define NUM_PIXELS 1

int powPin = NEOPIXEL_POWER;
int neoPin = PIN_NEOPIXEL;
int boardLed = PIN_LED_R;
int ledCount = 2;
int cyclePhase[3] = {7, 14, 7};
int ledPins[2] = {D0, D3}; // surface mount LEDs (D6 is not connected to the ground)
int enPin = D7;
int dirPin = D6;
int stepPin = D5;
int stepAmount = 400;
int microSteps = 16; // 16 means 1/16, 32 means 1/32 etc.
bool ledState = LOW; // HIGH, true, and 1 mean the same
bool btnState = HIGH; // button is high as it is connected to 3.3V via a pull-up resistor
bool inCycle = LOW;
bool isWaiting = HIGH;

Adafruit_NeoPixel pixels(NUM_PIXELS, neoPin, NEO_GRB + NEO_KHZ800);

unsigned long previousMillis = 0;
unsigned long interval = 1000; // interval in milliseconds

void setup() {
  pixels.begin();

  pinMode(PIN_LED_R, OUTPUT);
  pinMode(powPin, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enPin, OUTPUT);

  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  // Set the initial state of our LEDs
  for (int i = 0; i < ledCount; i++) {
    digitalWrite(ledPins[i], LOW);
  }

  // digitalWrite(PIN_LED_R, HIGH);

  digitalWrite(powPin, HIGH);
  digitalWrite(boardLed, LOW);
  digitalWrite(dirPin, HIGH);
  digitalWrite(enPin, LOW);

  Serial.begin(9600); // open the serial port at 9600 bps
}

void loop() {

  // Non-blocking button press detection
  bool btnReading = digitalRead(BTN);
  if (btnReading != btnState) {
    if (btnReading == LOW) { // LOW means button is pressed
      Serial.println("Button pressed >>>");
      isWaiting = LOW;
      btnState = LOW;
      inCycle = HIGH;
    } else {
      btnState = HIGH;
    }
    delay(50); // debounce
  }

  if (isWaiting) {
    waiting();
  } else if (inCycle) {
    // Serial.println("Entering your cycle >>>");
    cycle();
  }
}

void waiting() {
  static unsigned long waitMillis = 0;
  static int brightness = 0;
  static bool increasing = true;
  unsigned long currentMillis = millis();

  if (currentMillis - waitMillis >= 10) {
    waitMillis = currentMillis;
    if (increasing) {
      brightness++;
      if (brightness >= 50) increasing = false;
    } else {
      brightness--;
      if (brightness <= 0) increasing = true;
    }

    int r = random(0, 255);
    int g = 0;
    int b = 0;

    pixels.clear();
    pixels.setPixelColor(0, pixels.Color(r, g, b));
    pixels.setBrightness(brightness);
    pixels.show();
  }
}

void cycle() {
  static int dayCount = 0;
  static int phase = 1;
  static unsigned long cycleMillis = 0;
  unsigned long currentMillis = millis();

  pixels.clear();
  pixels.setPixelColor(0, 0);
  pixels.setBrightness(0);
  pixels.show();

  if (currentMillis - cycleMillis >= 800) {
    cycleMillis = currentMillis;
    dayCount++;
    Serial.print("Day count: ");
    Serial.println(dayCount);

    digitalWrite(boardLed, HIGH);
    oneFullRotation();
    delay(500); // brief delay to simulate blink
    digitalWrite(boardLed, LOW);

    if (phase == 1 && dayCount >= cyclePhase[0]) {
      phase++;
      ovulation();
    } else if (phase == 2 && dayCount >= cyclePhase[0] + cyclePhase[1]) {
      phase++;
      bleed();
    } else if (phase == 3 && dayCount >= cyclePhase[0] + cyclePhase[1] + cyclePhase[2]) {
      Serial.println("End of cycle >>>");
      inCycle = LOW;
      isWaiting = HIGH;
      phase = 1;
      dayCount = 0;
      digitalWrite(boardLed, LOW);
    }
  }
}

void ovulation() {
  Serial.println("Ovulation >>>");
  
  int ovary = random(0, 2);
  Serial.print("Ovary: ");
  Serial.println(ovary);

  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPins[ovary], HIGH);
    delay(100);
    digitalWrite(ledPins[ovary], LOW);
    delay(100);
  }
}

void bleed() {
  Serial.println("Bleeding >>>");
  int r = 255;
  int g = 0;
  int b = 0;
  for (int i = 0; i < 255; i++) {
    pixels.clear();
    pixels.setPixelColor(0, pixels.Color(r, g, b));
    pixels.setBrightness(i);
    pixels.show();
    delay(10);
  }
  for (int i = 255; i > 0; i--) {
    pixels.clear();
    pixels.setPixelColor(0, pixels.Color(r, g, b));
    pixels.setBrightness(i);
    pixels.show();
    delay(10);
  }

  oneFullRotation();
  delay(500);
}

void oneStep() {
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(100);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(100);
}

void oneFullRotation() {
  for (int i = 0; i < stepAmount * microSteps; i++) {
    oneStep();
  }
}