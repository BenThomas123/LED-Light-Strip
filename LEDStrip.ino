#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int ledPins[] = {0, 1, 2, 3, 4, 5, 6};     // Array of LED pins
const int buttonPins[] = {7, 8, 9, 10, 11, 12, 13};  // Array of button pins
const String colors[] = {"Red", "Blue", "Green", "Yellow", "Orange", "Purple", "White"};
const int numPairs = sizeof(ledPins) / sizeof(ledPins[0]);  // Number of LED-button pairs

int buttonStates[numPairs] = {0};       // Current button states
int lastButtonStates[numPairs] = {0};   // Previous button states
int ledStates[numPairs] = {LOW};        // Current LED states
int lastLedStates[numPairs] = {LOW};    // Previous LED states

unsigned long lastDebounceTime[numPairs] = {0};  // Last time each button was debounced
unsigned long debounceDelay = 50;                // Debounce delay in milliseconds

bool isOn[numPairs] = {false};         // Flags to track if each LED is turned on or off
bool isFlashing[numPairs] = {false};   // Flags to track if each LED is flashing

unsigned long flashInterval = 500;  // Flashing interval in milliseconds
unsigned long previousMillis[numPairs] = {0};   // Time since last LED state change

void setup() {
  lcd.init();
  lcd.backlight();
  for (int i = 0; i < numPairs; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Color: ");
  for (int i = 0; i < numPairs; i++) {
    int reading = digitalRead(buttonPins[i]);
    if (reading != lastButtonStates[i]) {
      lastDebounceTime[i] = millis();
    }

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading != buttonStates[i]) {
        buttonStates[i] = reading;
        // Button clicked
        if (buttonStates[i] == LOW) {
          // Toggle LED state based on current state
          if (!isOn[i]) {
            isOn[i] = true;
            isFlashing[i] = false;
            ledStates[i] = HIGH;  // Turn on the LED
          } else if (!isFlashing[i]) {
            isFlashing[i] = true;
            ledStates[i] = HIGH;  // Turn on the LED
            previousMillis[i] = millis();
          } else {
            isOn[i] = false;
            isFlashing[i] = false;
            ledStates[i] = LOW;   // Turn off the LED
          }

          // Update LED state
          lcd.clear();
          digitalWrite(ledPins[i], ledStates[i]);
          lcd.setCursor(0, 1);
          lcd.print(colors[i]);

        }
      }
    }

    if (isFlashing[i]) {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis[i] >= flashInterval) {
        previousMillis[i] = currentMillis;
        ledStates[i] = !ledStates[i];  // Toggle LED state
        digitalWrite(ledPins[i], ledStates[i]);
      }
    }

    // Save the current button state for comparison
    lastButtonStates[i] = reading;
  }
}