#include "bsp/BSP.h"

int currentMode = MODE_OFF;
bool systemOn = false;

int buttonState = HIGH;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// ===========================
// Inicialización
// ===========================
void Button_Init() {
    GPIO_PullUp(BUTTON_PIN);
}

// ===========================
// Actualizar estado del botón
// ===========================
void Button_Update() {
    int reading = GPIO_Read(BUTTON_PIN);

    // Debounce
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {

        if (reading != buttonState) {
            buttonState = reading;

            // PRESIONADO (LOW)
            if (buttonState == LOW) {
                systemOn = !systemOn;

                if (systemOn) {
                    Serial.println("Sistema ENCENDIDO");
                    currentMode = MODE_LOWPOWER;
                } else {
                    Serial.println("Sistema APAGADO");
                    currentMode = MODE_OFF;
                }
            }
        }
    }

    lastButtonState = reading;
}
