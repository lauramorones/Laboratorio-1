#include "bsp/BSP.h"

void LED_Init() {
  GPIO_Init(LED_PIN, OUTPUT);
  GPIO_Init(LED_ML_PIN, OUTPUT);  // Ambos LEDs inicializados igual
}

void LED_On() {
  GPIO_Write(LED_PIN, HIGH);
  GPIO_Write(LED_ML_PIN, HIGH);
}

void LED_Off() {
  GPIO_Write(LED_PIN, LOW);
  GPIO_Write(LED_ML_PIN, LOW);
}
