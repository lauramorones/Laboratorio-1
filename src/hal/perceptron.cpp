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

    r += w[N_inputs]; // bias
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
        vTaskDelay(40 / portTICK_PERIOD_MS);
        #else
        delay(40);
        #endif
    }
    return Serial.parseInt();
}

// ==========================
// Detección robusta de potenciómetros
// ==========================
// Detecta un pot si su lectura es ESTABLE entre lecturas
int DETECT_Pots() {

    int count = 0;

    for (int i = 0; i < N_DIMENSIONS; i++) {

        int minVal = 99999;
        int maxVal = -99999;

        // 10 lecturas por cada pin
        for (int r = 0; r < 10; r++) {
            int raw = analogRead(potPins[i]);
            if (raw < minVal) minVal = raw;
            if (raw > maxVal) maxVal = raw;
            delay(2);
        }

        int delta = maxVal - minVal;

        // Si el delta es chico, está conectado (≈5% o menos)
        if (delta < (ADCMAX * 0.05f)) {
            count++;
        }
    }

    return count;
}

// ==========================
// Entrenamiento
// ==========================

void PERCEPTRON_Init_Training() {

    // Inicializar ADCs
    ADC_Init(POT1_PIN);
    ADC_Init(POT2_PIN);
    ADC_Init(POT3_PIN);
    ADC_Init(POT4_PIN);
    ADC_Init(POT5_PIN);

    PRINT_Mensaje("===== PERCEPTRON SIMPLE EMBEBIDO =====");
    PRINT_Mensaje("Detectando potenciómetros...");

    // ==========================
    // Espera hasta que haya >= 1 pot
    // ==========================
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

    // ==========================
    // Inicialización de pesos
    // ==========================
    srand(0);
    for (int i = 0; i < N_DIMENSIONS + 1; i++)
        w[i] = (float)rand() / (float)RAND_MAX;

    // ==========================
    // Generar tabla de verdad
    // ==========================
    int_generate_truth_table();
    int rows = 1 << N_inputs;

    // ==========================
    // Selección de función
    // ==========================
    Serial.println();
    PRINT_Mensaje("Seleccione la función:");
    PRINT_Mensaje("0 = AND");
    PRINT_Mensaje("1 = OR");
    PRINT_Mensaje("2 = Personalizada DECIMAL");

    int sel = int_read_serial_int();
    Serial.println(sel);

    switch (sel) {

        // --------------------
        // AND
        // --------------------
        case 0:
            PRINT_Mensaje("Entrenando para AND...");
            for (int i = 0; i < rows; i++) {
                y[i] = 1;
                for (int k = 0; k < N_inputs; k++)
                    if (X[i][k] == 0) { y[i] = 0; break; }
            }
            break;

        // --------------------
        // OR
        // --------------------
        case 1:
            PRINT_Mensaje("Entrenando para OR...");
            for (int i = 0; i < rows; i++) {
                y[i] = 0;
                for (int k = 0; k < N_inputs; k++)
                    if (X[i][k] == 1) { y[i] = 1; break; }
            }
            break;

        // --------------------
        // PERSONALIZADA (NÚMERO DECIMAL)
        // --------------------
        case 2: {

            PRINT_Mensaje("Modo Personalizado DECIMAL");
            PRINT_Mensaje("Ingrese un solo numero entero decimal");
            PRINT_Mensaje("que represente la tabla de verdad completa.");

            int max_val = (1 << rows) - 1;

            Serial.print("Numero maximo permitido: ");
            Serial.println(max_val);

            Serial.print("Ingrese el numero decimal: ");
            int dec = int_read_serial_int();
            Serial.println(dec);

            if (dec < 0 || dec > max_val) {
                PRINT_Mensaje("Valor fuera de rango. Reiniciando...");
                delay(1200);
                #if defined(ESP32)
                    esp_restart();
                #else
                    asm volatile("jmp 0");
                #endif
            }

            // Convertir decimal --> bits
            for (int i = 0; i < rows; i++) {
                int bit_index = rows - 1 - i;
                y[i] = (dec >> bit_index) & 1;
            }

            // Mostrar tabla
            Serial.println("Tabla personalizada:");
            for (int i = 0; i < rows; i++) {
                Serial.print("Entrada [");
                for (int k = 0; k < N_inputs; k++)
                    Serial.print(X[i][k]);
                Serial.print("] => ");
                Serial.println(y[i]);
            }

            break;
        }

        default:
            PRINT_Mensaje("Opción inválida. Reiniciando...");
            delay(1200);
            #if defined(ESP32)
                esp_restart();
            #else
                asm volatile("jmp 0");
            #endif
    }

    // ==========================
    // ENTRENAMIENTO
    // ==========================

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

        if (totalError == 0.0f)
            break;

    } while (j < itmax);

    // Mostrar resultados
    PRINT_Perceptron_Weights(w);

    Serial.print("Iteraciones: ");
    Serial.println(j);

    g_trained = true;

    PRINT_Mensaje("===== Entrenamiento Completo =====");
}

// ==========================
// Ejecución
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
