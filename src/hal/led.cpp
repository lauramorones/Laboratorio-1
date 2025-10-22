#include "bsp/BSP.h"

void LED_Init() {
  GPIO_Init(LED_PIN, OUTPUT);
  GPIO_Init(LED_ML_PIN, OUTPUT);  // inicializa el LED del ML
}

void LED_On() {
  GPIO_Write(LED_PIN, HIGH);
}

void LED_Off() {
  GPIO_Write(LED_PIN, LOW);
}
