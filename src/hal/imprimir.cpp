#include "bsp/BSP.h"

// Imprime un mensaje general
void PRINT_Mensaje(const char* msg) {
  Serial.println(msg);
}

// Imprime el estado actual de las entradas y la salida
void PRINT_Perceptron_Status(int* inputs, int output) {
    Serial.print("Entradas: [");
    for(int i = 0; i < N_DIMENSIONS; i++) {
        Serial.print(inputs[i]);
        if (i < N_DIMENSIONS - 1) Serial.print(", ");
    }
    Serial.print("] => Salida: ");
    Serial.println(output);
}

// Imprime los pesos finales post-entrenamiento
void PRINT_Perceptron_Weights(float* weights) {
    Serial.println("Pesos finales:");
    for (int i = 0; i < N_DIMENSIONS; i++) {
        // Reemplazo de: Serial.printf("w[%d] = %.3f\n", i+1, weights[i]);
        Serial.print("w[");
        Serial.print(i + 1);
        Serial.print("] = ");
        Serial.println(weights[i], 3); // 3 decimales
    }
    // Reemplazo de: Serial.printf("w[0] (bias) = %.3f\n", weights[N_DIMENSIONS]);
    Serial.print("w[0] (bias) = ");
    Serial.println(weights[N_DIMENSIONS], 3); // 3 decimales
}