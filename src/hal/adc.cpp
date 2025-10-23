#include "bsp/BSP.h"

// Inicializa un pin analógico
void ADC_Init(int pin) {
    GPIO_Init(pin, INPUT);  // Configura el pin como entrada
}

// Lee el valor de un sensor y lo convierte a su magnitud física
float ADC_Read(int pin) {
    int valor = analogRead(pin);                 // Valor digital crudo (0–1023 o 0–4095)
    float voltaje = (valor * VREF) / ADCMAX;     // Conversión a volts reales

    // === Sensor de Temperatura ===
    if (pin == TEMP_PIN) {
        // Suponiendo LM35 o potenciómetro calibrado para 0–40 °C en 0–VREF V
        // 0 V = 0 °C, VREF = 40 °C
        float temperatura = (voltaje / VREF) * 100.0;
        if (temperatura < 0) temperatura = 0;
        if (temperatura > 100) temperatura = 100;
        return temperatura;
    }

    // === Sensor de Humedad ===
    if (pin == HUM_PIN) {
        // Escala lineal 0 V = 0 %, VREF = 100 %
        float humedad = (voltaje / VREF) * 100.0;
        if (humedad < 0) humedad = 0;
        if (humedad > 100) humedad = 100;
        return humedad;
    }

    // === Sensor de Luz ===
    if (pin == LUZ_PIN) {
        // Escala lineal inversa: más voltaje → más luz
        float luz = (voltaje / VREF) * 100.0;
        if (luz < 0) luz = 0;
        if (luz > 100) luz = 100;
        return luz;
    }

    // Si no coincide con ningún sensor, devolver el voltaje directo
    return voltaje;
}
