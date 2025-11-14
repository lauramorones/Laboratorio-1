#include "bsp/BSP.h"

// Inicializa un pin analógico
void ADC_Init(int pin) {
    GPIO_Init(pin, INPUT); // Configura el pin como entrada
}