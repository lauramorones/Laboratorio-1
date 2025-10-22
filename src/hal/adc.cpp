#include "bsp/BSP.h"

// Inicializa un pin analógico
void ADC_Init(int pin) {
    GPIO_Init(pin, INPUT);  // Configura el pin como entrada
}

// Lee el valor de un potenciómetro y lo convierte a voltaje
float ADC_Read(int pin) {
    int valor = analogRead(pin);          // Lectura del ADC
    float voltaje = (valor * VREF) / ADCMAX; // Conversión a volts (0–5 V o 0–3.3 V según placa)
    return voltaje;
}
