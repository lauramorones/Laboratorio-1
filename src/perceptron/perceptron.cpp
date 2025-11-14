#include "perceptron/perceptron.h"
#include "bsp/BSP.h"

int N_inputs = 0;
float w[MAX_INPUTS+1];

// Lee potenciómetros como 0/1
void Read_Pots(int X[]) {
    int pins[5] = { POT1_PIN, POT2_PIN, POT3_PIN, POT4_PIN, POT5_PIN };

    for (int i = 0; i < N_inputs; i++) {
        float volt = ADC_Read(pins[i]);
        X[i] = (volt >= 2.5f ? 1 : 0);
    }
}

// Tabla de verdad
void Generate_TruthTable(int X[][MAX_INPUTS]) {
    for (int i = 0; i < (1 << N_inputs); i++) {
        for (int k = 0; k < N_inputs; k++) {
            X[i][k] = (i >> (N_inputs - k - 1)) & 1;
        }
    }
}

// Convierte decimal a bits
void Apply_DesiredOutput(int y[], int filas, unsigned long salida) {
    for (int i = 0; i < filas; i++)
        y[i] = (salida >> i) & 1;
}

// Entrena perceptrón
void Train_Perceptron(int X[][MAX_INPUTS], int y[], int filas) {
    float n = 0.5f;

    for (int i = 0; i < N_inputs+1; i++)
        w[i] = random(0,1000)/1000.0f;

    for (int iter = 0; iter < 500; iter++) {

        float totalError = 0;

        for (int i = 0; i < filas; i++) {

            float r = 0;
            for (int k = 0; k < N_inputs; k++)
                r += w[k] * X[i][k];
            r += w[N_inputs];

            int out = (r >= 0);
            float e = y[i] - out;
            totalError += abs(e);

            for (int k = 0; k < N_inputs; k++)
                w[k] += n * e * X[i][k];

            w[N_inputs] += n * e;
        }

        if (totalError == 0) break;
    }
}

int Perceptron_Predict(int X[]) {
    float r = 0;

    for (int i = 0; i < N_inputs; i++)
        r += w[i] * X[i];

    r += w[N_inputs];

    return (r >= 0 ? 1 : 0);
}
