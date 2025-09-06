#include "bsp/BSP.h"

// Inicializa un pin analógico
void ADC_Init(int pin) {
    GPIO_Init(pin, INPUT);  // Configura el pin como entrada
}

// Lee el valor de un sensor y lo convierte a su unidad
float ADC_Read(int pin) {
    int valor = analogRead(pin);                  // Lee valor del ADC
    float voltaje = (valor * VREF) / ADCMAX;     // Convierte a voltaje

    if (pin == TEMP_PIN) {
        return voltaje * 100.0;  // LM35: 10mV por °C
    }

    if (pin == HUM_PIN) {
        float hum = (voltaje / VREF) * 100.0;    // Humedad %
        if (hum < 0) hum = 0;
        if (hum > 100) hum = 100;
        return hum;
    }

    if (pin == LUZ_PIN) {
        float lux = (voltaje / VREF) * 100.0;    // Luz %
        if (lux < 0) lux = 0;
        if (lux > 100) lux = 100;
        return lux;
    }

    return 0.0; // Si no es ningún pin válido
}
