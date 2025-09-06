#include "bsp/BSP.h"

void GPIO_Init(uint8_t pin, uint8_t mode) {
  pinMode(pin, mode);
}

void GPIO_Write(uint8_t pin, uint8_t value) {
  digitalWrite(pin, value);
}

int GPIO_Read(uint8_t pin) {
  return digitalRead(pin);
}

void GPIO_PullUp(uint8_t pin) {
  pinMode(pin, INPUT_PULLUP);
}