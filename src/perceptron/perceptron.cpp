#include "perceptron/perceptron.h"
#include "bsp/BSP.h"

int N_inputs = 0;
float w[MAX_INPUTS + 1];

// =============================================
// Detecta cuántos potenciómetros están conectados
// =============================================
void Detect_Pots() {
    N_inputs = 0;

    int pins[5] = {A0, A1, A2, A3, A4};

    for (int i = 0; i < 5; i++) {
        int raw = analogRead(pins[i]);
        float volt = (raw * VREF) / ADCMAX;

        if (volt > 0.05f)
            N_inputs++;
    }

    Serial.print("Potenciómetros detectados: ");
    Serial.println(N_inputs);
}

// =============================================
// Lee potenciómetros y los convierte a 0 o 1
// =============================================
void Read_Pots(int X[]) {
    int pins[5] = {A0, A1, A2, A3, A4};

    for (int i = 0; i < N_inputs; i++) {
        float volt = analogRead(pins[i]) * VREF / ADCMAX;
        X[i] = (volt >= 2.5f) ? 1 : 0;
    }
}

// =============================================
// Genera la tabla de verdad de entradas según N_inputs
// =============================================
void Generate_TruthTable(int X[][MAX_INPUTS]) {
    int filas = 1 << N_inputs;

    for (int i = 0; i < filas; i++) {
        for (int k = 0; k < N_inputs; k++) {
            X[i][k] = (i >> (N_inputs - k - 1)) & 1;
        }
    }
}

// =============================================
// Aplica la salida deseada usando un número decimal
// =============================================
void Apply_DesiredOutput(int y[], int filas, unsigned long decimalSalida) {
    for (int i = 0; i < filas; i++)
        y[i] = (decimalSalida >> i) & 1;
}

// =============================================
// Entrenamiento del perceptrón
// =============================================
void Train_Perceptron(int X[][MAX_INPUTS], int y[], int filas) {
    float n = 0.5f;
    int it_max = 500;

    // Pesos aleatorios
    for (int i = 0; i < N_inputs + 1; i++)
        w[i] = random(0, 1000) / 1000.0f;

    for (int iter = 0; iter < it_max; iter++) {
        float totalError = 0;

        for (int i = 0; i < filas; i++) {
            float r = 0;

            for (int k = 0; k < N_inputs; k++)
                r += w[k] * X[i][k];

            r += w[N_inputs]; // bias

            int out = (r >= 0 ? 1 : 0);
            float e = y[i] - out;
            totalError += abs(e);

            for (int k = 0; k < N_inputs; k++)
                w[k] += n * e * X[i][k];

            w[N_inputs] += n * e;
        }

        if (totalError == 0)
            break;
    }

    Serial.println("=== Pesos finales ===");
    for (int i = 0; i < N_inputs + 1; i++) {
        Serial.print("w[");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(w[i], 4);
    }
}

// =============================================
// Predicción en tiempo real
// =============================================
int Perceptron_Predict(int X[]) {
    float r = 0;

    for (int i = 0; i < N_inputs; i++)
        r += w[i] * X[i];

    r += w[N_inputs];

    return (r >= 0 ? 1 : 0);
}
