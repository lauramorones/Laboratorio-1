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

//Inicializa el boton
void Button_Init() {
  GPIO_PullUp(BUTTON_PIN);
}

// ==========================
// Actualización con del estado del boton
// ==========================
void Button_Update() {
  int reading = digitalRead(BUTTON_PIN);

  // Si cambia el estado, reinicia el tiempo de rebote
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
          // 🔥 Forzar apagado inmediato y sincronizado
          currentMode = MODE_OFF;
          LED_Off();
          LED_ML_Off();             // Apaga el LED ML directamente
          vTaskDelay(100 / portTICK_PERIOD_MS); // pequeña pausa para asegurar apagado
          Serial.println("Sistema OFF -> LEDs apagados");
        }
      }
    }
  }

  lastButtonState = reading;
}

