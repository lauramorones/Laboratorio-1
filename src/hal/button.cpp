#include "bsp/BSP.h"

// ==========================
// Variables globales
// ==========================
int currentMode = MODE_OFF;
bool systemOn = false; 

int buttonState = HIGH;      // Estado estable del botón
int lastButtonState = HIGH;  // Estado anterior del botón
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // ms

// ==========================
// Inicialización
// ==========================
void Button_Init() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

// ==========================
// Actualización con debounce
// ==========================
void Button_Update() {
  int reading = digitalRead(BUTTON_PIN);

  // Si cambia el estado, reinicia el tiempo de debounce
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // Solo actualiza si ya pasó el tiempo de debounce
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // Si el botón fue presionado (LOW)
      if (buttonState == LOW) {
        systemOn = !systemOn;

        if (systemOn) {
          Serial.println("Sistema ON");
        } else {
          currentMode = MODE_OFF;
          Serial.println("Sistema OFF");
        }
      }
    }
  }

  lastButtonState = reading;
}
