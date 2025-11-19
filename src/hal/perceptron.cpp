#include "bsp/BSP.h"

// ==========================
// Variables globales
// ==========================

int N_inputs = 0;

// Pesos: hasta 5 entradas + bias
float w[N_DIMENSIONS + 1];

// Entradas X[][], salida lógica y_bin[], salida ADALINE con ±2.0: t[]
int   X[N_ROWS][N_DIMENSIONS];
int   y_bin[N_ROWS];   // 0/1 (humano)
float t[N_ROWS];       // -2/+2 (LMS ampliado)

int   g_current_inputs[N_DIMENSIONS];
int   g_perceptron_output = 0;
bool  g_trained = false;

const int potPins[N_DIMENSIONS] = { POT1_PIN, POT2_PIN, POT3_PIN, POT4_PIN, POT5_PIN };
const int potDetPins[N_DIMENSIONS] = { POT1_DET_PIN, POT2_DET_PIN, POT3_DET_PIN, POT4_DET_PIN, POT5_DET_PIN };

int activePotPins[N_DIMENSIONS];

// ==========================
// Funciones internas comunes
// ==========================

int bin_step(float x) {
    return (x >= 0.0f ? 1 : 0);
}

float raw_from_ints(int *inputs) {
    float r = 0.0f;
    for (int k = 0; k < N_inputs; k++) {
        r += w[k] * (float)inputs[k];
    }
    r += w[N_inputs]; // bias
    return r;
}

int model_predict(int *inputs) {
    float r = raw_from_ints(inputs);
    return bin_step(r);
}

// Generar tabla de verdad
void int_generate_truth_table() {
    int rows = 1 << N_inputs;
    for (int i = 0; i < rows; i++) {
        for (int k = 0; k < N_inputs; k++) {
            X[i][k] = (i >> (N_inputs - k - 1)) & 1;
        }
    }
}

// Leer integer del serial
int int_read_serial_int() {
    while (Serial.available() == 0) {
        #if defined(ESP32)
        vTaskDelay(40 / portTICK_PERIOD_MS);
        #else
        delay(40);
        #endif
    }
    return Serial.parseInt();
}

// Detectar potenciómetros
int DETECT_Pots() {
    N_inputs = 0;

    for (int i = 0; i < N_DIMENSIONS; i++) {
        pinMode(potDetPins[i], INPUT_PULLUP);
        delay(2);
        if (digitalRead(potDetPins[i]) == LOW) {
            activePotPins[N_inputs] = potPins[i];
            N_inputs++;
        }
    }
    return N_inputs;
}

// Inicializar pesos pequeños aleatorios
void init_weights() {
    srand(0);
    for (int i = 0; i < N_DIMENSIONS + 1; i++) {
        float r = ((float)rand() / RAND_MAX);
        w[i] = (r - 0.5f) * 0.2f;   // en [-0.1, +0.1]
    }
}

// ==============================
// DEBUG: Verificar tabla de verdad
// ==============================
static void DEBUG_TestTruthTable() {
    if (N_inputs <= 0) return;

    int rows = 1 << N_inputs;
    Serial.println();
    Serial.println("===== VERIFICACION TABLA DE VERDAD (MODELO) =====");
    Serial.println(" X[] -> y_bin | pred | r(lineal)");

    for (int i = 0; i < rows; i++) {

        float r = 0;
        for (int k = 0; k < N_inputs; k++) r += w[k] * X[i][k];
        r += w[N_inputs];

        int pred = (r >= 0 ? 1 : 0);

        Serial.print("X[");
        for (int k = 0; k < N_inputs; k++) Serial.print(X[i][k]);
        Serial.print("] -> y_bin=");
        Serial.print(y_bin[i]);
        Serial.print(" | pred=");
        Serial.print(pred);
        Serial.print(" | r=");
        Serial.println(r, 4);
    }
    Serial.println("===================================================");
}

// ==========================
// Entrenamiento ADALINE (LMS ±2.0) para 1-4 pots
// ==========================
void train_LMS() {
    PRINT_Mensaje("Usando ADALINE (LMS ±2.0) para entrenamiento...");

    int rows = 1 << N_inputs;
    float n = 0.1f;       // learning rate
    int epochs = 4000;

    for (int ep = 0; ep < epochs; ep++) {

        float mse = 0;

        for (int i = 0; i < rows; i++) {

            float r = 0;
            for (int k = 0; k < N_inputs; k++)
                r += w[k] * X[i][k];
            r += w[N_inputs];

            float error = t[i] - r;
            mse += error * error;

            // LMS update
            for (int k = 0; k < N_inputs; k++)
                w[k] += n * error * X[i][k];

            w[N_inputs] += n * error;   // bias
        }

        mse /= rows;

        // Verificar clasificación correcta
        int mis = 0;
        for (int i = 0; i < rows; i++) {
            float r = 0;
            for (int k = 0; k < N_inputs; k++) r += w[k] * X[i][k];
            r += w[N_inputs];

            int pred = (r >= 0 ? 1 : 0);
            if (pred != y_bin[i]) mis++;
        }

        if (mis == 0 && mse < 0.001f) break;
    }
}

// ==========================
// Entrenamiento Perceptrón clásico para 5 pots
// ==========================
void train_Perceptron() {
    PRINT_Mensaje("Usando Perceptron clasico para entrenamiento...");

    int rows = 1 << N_inputs;
    float n = 0.5f;
    int it_max = 3000;

    for (int epoch = 0; epoch < it_max; epoch++) {
        int totalError = 0;

        for (int i = 0; i < rows; i++) {
            float r = 0.0f;
            for (int k = 0; k < N_inputs; k++) {
                r += w[k] * (float)X[i][k];
            }
            r += w[N_inputs];

            int out = (r >= 0.0f) ? 1 : 0;
            int error = y_bin[i] - out; // 0, +1 o -1

            if (error != 0) {
                totalError += (error > 0) ? error : -error;

                // Regla del perceptron
                for (int k = 0; k < N_inputs; k++) {
                    w[k] += n * (float)error * (float)X[i][k];
                }
                w[N_inputs] += n * (float)error;
            }
        }

        if (totalError == 0) {
            break; // convergió perfectamente
        }
    }
}

// ==========================
// Construir salidas deseadas (y_bin y t)
// ==========================
void build_outputs(int sel) {

    int rows = 1 << N_inputs;

    switch (sel) {

        case 0: { // AND
            PRINT_Mensaje("Configurando salidas para AND...");
            for (int i = 0; i < rows; i++) {
                int salida = 1;
                for (int k = 0; k < N_inputs; k++)
                    if (X[i][k] == 0) salida = 0;

                y_bin[i] = salida;
                t[i]     = salida ? +2.0f : -2.0f;   // ±2.0 para LMS
            }
            break;
        }

        case 1: { // OR
            PRINT_Mensaje("Configurando salidas para OR...");
            for (int i = 0; i < rows; i++) {
                int salida = 0;
                for (int k = 0; k < N_inputs; k++)
                    if (X[i][k] == 1) salida = 1;

                y_bin[i] = salida;
                t[i]     = salida ? +2.0f : -2.0f;
            }
            break;
        }

        case 2: { // Decimal personalizado
            PRINT_Mensaje("Configurando salidas para funcion personalizada DECIMAL...");

            unsigned long dec = int_read_serial_int();
            Serial.print("Numero decimal recibido: ");
            Serial.println(dec);

            for (int i = 0; i < rows; i++) {
                int bitIndex = rows - 1 - i;
                int bit = (dec >> bitIndex) & 1U;

                y_bin[i] = bit;
                t[i]     = bit ? +2.0f : -2.0f;
            }

            Serial.println("Tabla personalizada (X -> y_bin):");
            for (int i = 0; i < rows; i++) {
                Serial.print("X[");
                for (int k = 0; k < N_inputs; k++) {
                    Serial.print(X[i][k]);
                }
                Serial.print("] => ");
                Serial.println(y_bin[i]);
            }

            break;
        }

        default:
            PRINT_Mensaje("Opción inválida. Reiniciando...");
            delay(1000);
            #if defined(ESP32)
            esp_restart();
            #else
            asm volatile("jmp 0");
            #endif
    }
}

// ==========================
// ENTRENAMIENTO (elige algoritmo según N_inputs)
// ==========================
void PERCEPTRON_Init_Training() {

    // 1) Inicializar ADC
    ADC_Init(POT1_PIN); ADC_Init(POT2_PIN); ADC_Init(POT3_PIN);
    ADC_Init(POT4_PIN); ADC_Init(POT5_PIN);

    PRINT_Mensaje("===== MODELO HIBRIDO LMS/PERCEPTRON =====");
    PRINT_Mensaje("Detectando potenciómetros...");

    // 2) Esperar al menos 1 pot
    while (true) {
        if (DETECT_Pots() > 0) break;

        PRINT_Mensaje("0 pots detectados -> LOW POWER");
        LED_Off();

        #if defined(ESP32)
        vTaskDelay(1200 / portTICK_PERIOD_MS);
        #else
        delay(1200);
        #endif
    }

    Serial.print("Potenciómetros detectados: ");
    Serial.println(N_inputs);

    // 3) Inicializar pesos
    init_weights();

    // 4) Tabla de verdad
    int_generate_truth_table();
    int rows = 1 << N_inputs;
    (void)rows; // silenciar warning si no se usa

    // 5) Pedir funcion
    Serial.println();
    PRINT_Mensaje("Seleccione la función:");
    PRINT_Mensaje("0 = AND");
    PRINT_Mensaje("1 = OR");
    PRINT_Mensaje("2 = Personalizada DECIMAL");

    int sel = int_read_serial_int();
    Serial.println(sel);

    // 6) Construir salidas deseadas
    build_outputs(sel);

    // 7) Elegir algoritmo según N_inputs
    if (N_inputs <= 4) {
        train_LMS();
    } else {
        train_Perceptron();
    }

    // 8) Resultados
    PRINT_Perceptron_Weights(w);
    PRINT_Mensaje("===== ENTRENAMIENTO COMPLETO =====");

    DEBUG_TestTruthTable();

    g_trained = true;
}

// ==========================
// RUN-TIME DEL MODELO
// ==========================
void PERCEPTRON_Run_Update() {

    if (!g_trained || N_inputs <= 0) return;

    for (int i = 0; i < N_inputs; i++) {
        int raw = analogRead(activePotPins[i]);
        g_current_inputs[i] = (raw >= ADC_THRESHOLD) ? 1 : 0;
    }

    g_perceptron_output = model_predict(g_current_inputs);

    PRINT_Perceptron_Status(g_current_inputs, g_perceptron_output);
}

int PERCEPTRON_Get_Output() {
    return g_perceptron_output;
}
