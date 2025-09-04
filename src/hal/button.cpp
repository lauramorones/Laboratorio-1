#include "bsp/BSP.h"

// Variables globales
int currentMode = MODE_OFF;
bool systemOn = false;

static unsigned long lastDebounceTime = 0;
static const unsigned long debounceDelay = 50;
static int lastButtonState = HIGH;
static int buttonState = HIGH;

void Button_Init() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void Button_Update() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        systemOn = !systemOn;

        if (!systemOn) {
          currentMode = MODE_OFF;
          Serial.println("Sistema OFF");
        } else {
          Serial.println("Sistema ON");
        }
      }
    }
  }

  lastButtonState = reading;
}
