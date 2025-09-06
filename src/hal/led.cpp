#include "bsp/BSP.h"

void LED_Init() {
  GPIO_Init(LED_PIN, OUTPUT);
}

void LED_On() {
  GPIO_Write(LED_PIN, HIGH);
}

void LED_Off() {
  GPIO_Write(LED_PIN, LOW);
}