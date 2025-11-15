#include "bsp/BSP.h"

// Imprime un mensaje general
void PRINT_Mensaje(const char* msg) {
  Serial.println(msg);
}

// Imprime el estado actual de las entradas y la salida
void PRINT_Perceptron_Status(int* inputs, int output) {
    Serial.print("Entradas detectadas (");
    Serial.print(N_inputs);
    Serial.print("): [");
    for (int i = 0; i < N_inputs; i++) {
        Serial.print(inputs[i]);
        if (i < N_inputs - 1) Serial.print(", ");
    }
    Serial.print("] => Salida: ");
    Serial.println(output);
}

// Imprime los pesos finales post-entrenamiento
void PRINT_Perceptron_Weights(float* weights) {
    Serial.print("Potenciómetros utilizados en el modelo: ");
    Serial.println(N_inputs);

    Serial.println("Pesos finales:");
    for (int i = 0; i < N_inputs; i++) {
        Serial.print("w[");
        Serial.print(i + 1);
        Serial.print("] = ");
        Serial.println(weights[i], 3); // 3 decimales
    }
    Serial.print("w[0] (bias) = ");
    Serial.println(weights[N_inputs], 3); // 3 decimales
}
