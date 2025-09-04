#include "bsp/BSP.h"

void LED_Init() {
  pinMode(LED_PIN, OUTPUT);
}

void LED_On() {
  digitalWrite(LED_PIN, HIGH);
}

void LED_Off() {
  digitalWrite(LED_PIN, LOW);
}