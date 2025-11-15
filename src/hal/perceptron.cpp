#include "bsp/BSP.h"

// ==========================
// Variables globales
// ==========================

int N_inputs = 0;

float w[N_DIMENSIONS + 1];
int   X[N_ROWS][N_DIMENSIONS];
int   y[N_ROWS];

int   g_current_inputs[N_DIMENSIONS];
int   g_perceptron_output = 0;
bool  g_trained = false;

const int potPins[N_DIMENSIONS] = {
    POT1_PIN, POT2_PIN, POT3_PIN, POT4_PIN, POT5_PIN
};

// ==========================
// Funciones internas
// ==========================

int int_salidaBin(float x) {
    return (x >= 0) ? 1 : 0;
}

int int_run_model(int* inputs) {
    float r = 0.0f;
    for (int k = 0; k < N_inputs; k++)
        r += w[k] * (float)inputs[k];

    r += w[N_inputs];
    return int_salidaBin(r);
}

void int_generate_truth_table() {
    int rows = 1 << N_inputs;

    for (int i = 0; i < rows; i++) {
        for (int k = 0; k < N_inputs; k++) {
            X[i][k] = (i >> (N_inputs - k - 1)) & 1;
        }
    }
}

int int_read_serial_int() {
    while (Serial.available() == 0) {
        #if defined(ESP32)
        vTaskDelay(50 / portTICK_PERIOD_MS);
        #else
        delay(50);
        #endif
    }
    return Serial.parseInt();
}

// ==========================
// Detección de potenciómetros
// ==========================

int DETECT_Pots() {
    int count = 0;

    for (int i = 0; i < N_DIMENSIONS; i++) {

        int minVal = 99999;
        int maxVal = -99999;

        // 10 lecturas rápidas
        for (int r = 0; r < 10; r++) {
            int raw = analogRead(potPins[i]);
            if (raw < minVal) minVal = raw;
            if (raw > maxVal) maxVal = raw;
            delay(2);
        }

        int delta = maxVal - minVal;

        // Si la lectura es estable, se asume que hay un pot conectado
        if (delta < (ADCMAX * 0.05)) {   // 5% de tolerancia
            count++;
        }
    }

    return count;
}


// ==========================
// Entrenamiento
// ==========================

void PERCEPTRON_Init_Training() {

    ADC_Init(POT1_PIN);
    ADC_Init(POT2_PIN);
    ADC_Init(POT3_PIN);
    ADC_Init(POT4_PIN);
    ADC_Init(POT5_PIN);

    PRINT_Mensaje("----- PERCEPTRON SIMPLE BINARIO -----");
    PRINT_Mensaje("Detectando potenciómetros...");

    // Espera hasta que detecte al menos uno
    while (true) {
        N_inputs = DETECT_Pots();

        if (N_inputs > 0) {
            Serial.print("Potenciómetros detectados: ");
            Serial.println(N_inputs);
            break;
        }

        PRINT_Mensaje("0 potenciometros detectados -> MODO LOW POWER");
        PRINT_Mensaje("Conecte al menos 1 potenciómetro...");

        LED_Off();

        #if defined(ESP32)
        vTaskDelay(1200 / portTICK_PERIOD_MS);
        #else
        delay(1200);
        #endif
    }

    Serial.print("Entradas activas: ");
    Serial.println(N_inputs);

    // Inicializar pesos aleatorios
    srand(0);
    for (int i = 0; i < N_DIMENSIONS + 1; i++)
        w[i] = (float)rand() / (float)RAND_MAX;

    // Generar tabla de verdad solo para N_inputs
    int_generate_truth_table();
    int rows = 1 << N_inputs;

    PRINT_Mensaje("Seleccione la función (0=AND, 1=OR, 2=Personalizada): ");
    int sel = int_read_serial_int();
    Serial.println(sel);

    switch (sel) {

        case 0:
            PRINT_Mensaje("Entrenando para AND...");
            for (int i = 0; i < rows; i++) {
                y[i] = 1;
                for (int k = 0; k < N_inputs; k++)
                    if (X[i][k] == 0) { y[i] = 0; break; }
            }
            break;

        case 1:
            PRINT_Mensaje("Entrenando para OR...");
            for (int i = 0; i < rows; i++) {
                y[i] = 0;
                for (int k = 0; k < N_inputs; k++)
                    if (X[i][k] == 1) { y[i] = 1; break; }
            }
            break;

        case 2:
            PRINT_Mensaje("Entradas personalizadas:");
            for (int i = 0; i < rows; i++) {
                Serial.print("[");
                Serial.print(i);
                Serial.print("] ");

                for (int k = 0; k < N_inputs; k++) {
                    Serial.print(X[i][k]);
                    Serial.print(" ");
                }

                Serial.print("=> ");
                y[i] = int_read_serial_int();
                Serial.println(y[i]);
            }
            break;

        default:
            PRINT_Mensaje("Opcion invalida. Reiniciando...");
            delay(1000);
            #if defined(ESP32)
            esp_restart();
            #else
            asm volatile ("jmp 0");
            #endif
    }

    // Entrenamiento
    PRINT_Mensaje("Entrenando perceptrón...");
    float n = 0.5f;
    int itmax = 500;
    int j = 0;

    do {
        float totalError = 0.0f;
        j++;

        for (int i = 0; i < rows; i++) {

            float r = 0.0f;
            for (int k = 0; k < N_inputs; k++)
                r += w[k] * X[i][k];

            r += w[N_inputs];

            int out = int_salidaBin(r);
            float error = (float)y[i] - (float)out;

            totalError += fabsf(error);

            for (int k = 0; k < N_inputs; k++)
                w[k] += n * error * X[i][k];

            w[N_inputs] += n * error;
        }

        if (totalError == 0.0f) break;

    } while (j < itmax);

    PRINT_Perceptron_Weights(w);

    Serial.print("Iteraciones: ");
    Serial.println(j);

    g_trained = true;

    PRINT_Mensaje("===== Entrenamiento Completo =====");
}

// ==========================
// Ejecución del modelo
// ==========================

void PERCEPTRON_Run_Update() {

    if (!g_trained || N_inputs <= 0)
        return;

    for (int i = 0; i < N_inputs; i++) {
        int raw = analogRead(potPins[i]);
        g_current_inputs[i] = (raw >= ADC_THRESHOLD) ? 1 : 0;
    }

    g_perceptron_output = int_run_model(g_current_inputs);

    PRINT_Perceptron_Status(g_current_inputs, g_perceptron_output);
}

int PERCEPTRON_Get_Output() {
    return g_perceptron_output;
}
