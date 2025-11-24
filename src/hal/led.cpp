#include "bsp/BSP.h"

// ==================== LED DEL PERCEPTRON =====================
void LED_PERCEPTRON_Init() {
  GPIO_Init(LED_PERC_PIN, OUTPUT);
}

void LED_PERCEPTRON_On() {
  GPIO_Write(LED_PERC_PIN, HIGH);
}

void LED_PERCEPTRON_Off() {
  GPIO_Write(LED_PERC_PIN, LOW);
}

// ==================== LED DE MODO DEL SISTEMA =====================
void LED_MODE_Init() {
  GPIO_Init(LED_MODE_PIN, OUTPUT);
}

void LED_MODE_On() {
  GPIO_Write(LED_MODE_PIN, HIGH);
}

void LED_MODE_Off() {
  GPIO_Write(LED_MODE_PIN, LOW);
}
