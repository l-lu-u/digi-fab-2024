#include <Adafruit_NeoPixel.h>

#define BTN D1
#define NUM_PIXELS 1

int powPin = NEOPIXEL_POWER;
int neoPin = PIN_NEOPIXEL;
int boardLed = PIN_LED_G;
int ledCount = 2;
int cyclePhase[3] = {7, 14, 7};
int ledPins[2] = {D0, D7}; // surface mount LEDs (D6 is not connected to the ground)
bool ledState = LOW; // HIGH, true, and 1 mean the same
bool btnState = HIGH; // button is high as it is connected to 3.3V via a pull-up resistor
bool inCycle = LOW;
bool isWaiting = HIGH;

Adafruit_NeoPixel pixels(NUM_PIXELS, neoPin, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();

  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(powPin, OUTPUT);

  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  pinMode(BTN, INPUT_PULLUP);

  // Set the initial state of our LEDs
  for (int i = 0; i < ledCount; i++) {
    digitalWrite(ledPins[i], LOW);
  }

  digitalWrite(PIN_LED_R, HIGH);
  digitalWrite(PIN_LED_G, HIGH);
  digitalWrite(PIN_LED_B, HIGH);

  digitalWrite(powPin, HIGH);
  digitalWrite(boardLed, HIGH);

  Serial.begin(9600); // open the serial port at 9600 bps:
}

void loop() {

  /* A board that simulates basic menstruation*/
  // 1. Press the button to start
  // 2. boardLed (R) blink as the count
  // 3. randomly blink one of the LEDs at #7
  // 4. continue blinding the boardLed until #21
  // 5. change the pixels to dark read colour, boardLed count 7 more times
  // 6. end to the basic state, boardLed R to G, pixels fading

  bool btnReading = digitalRead(BTN);

  if (isWaiting){
    waiting();
  }

  // Check if the button state has changed
  if (btnReading != btnState) {
    if (btnReading == LOW) { // LOW means button is pressed
      Serial.println("Button pressed >>>");
      isWaiting = LOW;
      btnState = LOW;
      inCycle = HIGH;
    }
    delay(50); // debounce
    cycle();
    isWaiting = HIGH;
  } else {
      // Button released
      btnState = HIGH;
  }

}

void waiting(){
  int r = random(0,255);
  int g = random(0,255);
  int b = random(0,255);
  if (!inCycle && btnState==HIGH) {
    for (int i=0; i<50; i++){
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(r, g, b));
      pixels.setBrightness(i);
      pixels.show();
      delay(10);
    }
    for (int i=50; i>0; i--){
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(r, g, b));
      pixels.setBrightness(i);
      pixels.show();
      delay(10);
    }
  }
}

void cycle(){
  Serial.println("Entering your cycle >>>");
  boardLed = PIN_LED_R;

  int dayCount = 0;
  
  for (int i=0; i<cyclePhase[1]; i++){
    dayCount++;
    Serial.print("Day count: ");
    Serial.println(dayCount);

    digitalWrite(boardLed, HIGH);
    delay(500);
    digitalWrite(boardLed, LOW);
    delay(300);  
  }

  ovulation();

  for (int i=0; i<cyclePhase[2]; i++){
    dayCount++;
    Serial.print("Day count: ");
    Serial.println(dayCount);

    digitalWrite(boardLed, HIGH);
    delay(500);
    digitalWrite(boardLed, LOW);
    delay(300);  
  }

  bleed();
  Serial.println("End of cycle >>>");
  inCycle = LOW;
}

void ovulation(){
  Serial.println("Ovulation >>>");
  
  int ovary = random(0, 2);
  Serial.print("Ovary: ");
  Serial.println(ovary);

  for (int i=0; i<5; i++) {
    digitalWrite(ledPins[ovary], HIGH);
    delay(100);
    digitalWrite(ledPins[ovary], LOW);
    delay(100);
  }
}

void bleed(){
  Serial.println("Bleeding >>>");
  int r = random(0, 100);
  int g = 0;
  int b = 0;
  for (int i=0; i<100; i++){
    pixels.clear();
    pixels.setPixelColor(0, pixels.Color(r, g, b));
    pixels.setBrightness(i);
    pixels.show();
    delay(10);
  }
  for (int i=100; i>0; i--){
    pixels.clear();
    pixels.setPixelColor(0, pixels.Color(r, g, b));
    pixels.setBrightness(i);
    pixels.show();
    delay(10);
  }
}