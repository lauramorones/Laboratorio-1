#include "bsp/BSP.h"

void ADC_Init(int pin) {
    pinMode(pin, INPUT);
}

float ADC_Read(int pin) {
    int raw = analogRead(pin);
    return (raw * VREF) / ADCMAX;
}
