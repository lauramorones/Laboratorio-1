#include "bsp/BSP.h"

// ==========================
// Variables globales
// ==========================
int currentMode = MODE_OFF;
bool systemOn = false;

// Flags de control de flujo
bool requestRetrain = false;      // Pedido de reinicio completo (nuevo entrenamiento)
bool hasRunAfterTraining = false; // Indica si ya hubo una corrida después del último entrenamiento

int buttonState = HIGH;      
int lastButtonState = HIGH;  
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // ms

// Inicializa el botón
void Button_Init() {
    GPIO_PullUp(BUTTON_PIN);
}

// ==========================
// Actualización del estado del botón
// ==========================
void Button_Update() {
    int reading = GPIO_Read(BUTTON_PIN);

    // Si cambia el estado, reinicia el tiempo de debounce
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    // Solo actualiza si ya pasó el tiempo de debounce
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;

            // Botón presionado (activo en LOW)
            if (buttonState == LOW) {

                // Caso 1: el sistema está corriendo -> apagar
                if (systemOn) {
                    systemOn = false;
                    currentMode = MODE_OFF;
                    Serial.println("Sistema OFF (deteniendo ejecucion)");
                }
                else {
                    // Sistema apagado
                    if (!hasRunAfterTraining) {
                        // Caso 2: acabamos de entrenar y aún no hemos corrido
                        // Primer botón: INICIAR EJECUCIÓN (NO reentrena)
                        systemOn = true;
                        currentMode = MODE_RUN;
                        hasRunAfterTraining = true;
                        Serial.println("Sistema ON (inicio de ejecucion despues de entrenamiento)");
                    } else {
                        // Caso 3: ya corrió al menos una vez y está apagado
                        // Siguiente botón: pedir REENTRENAMIENTO COMPLETO
                        requestRetrain = true;
                        systemOn = false;
                        currentMode = MODE_OFF;
                        Serial.println("Solicitud de reinicio completo (nuevo entrenamiento)");
                    }
                }
            }
        }
    }

    lastButtonState = reading;
}
