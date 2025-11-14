#include "bsp/BSP.h"

// ==========================
// Variables globales del Perceptrón
// ==========================
float w[N_DIMENSIONS + 1];      // N pesos + W0 (bias)
int   X[N_ROWS][N_DIMENSIONS];  // Matriz de entradas (Tabla de verdad)
int   y[N_ROWS];                // Salidas esperadas
int   g_current_inputs[N_DIMENSIONS]; // Entradas actuales de los POTs
int   g_perceptron_output = 0;        // Salida actual del modelo
bool  g_trained = false;        // Flag para saber si el modelo está listo

// ==========================
// Funciones Internas del Perceptrón
// ==========================

// Función de activación binaria
int int_salidaBin(float x) {
    if (x >= 0) {
        return 1;
    } else {
        return 0;
    }
}

// Ejecuta el modelo con los pesos ya entrenados
int int_run_model(int* inputs) {
    float r = 0.0f;
    for (int k = 0; k < N_DIMENSIONS; k++)
        r += w[k] * (float)inputs[k];
    r += w[N_DIMENSIONS]; // bias
    return int_salidaBin(r);
}

// Genera la tabla de verdad (entradas X)
void int_generate_truth_table() {
    for (int i = 0; i < N_ROWS; i++) {
        for (int k = 0; k < N_DIMENSIONS; k++) {
            X[i][k] = (i >> (N_DIMENSIONS - k - 1)) & 1;
        }
    }
}

// Espera un input numérico desde el Monitor Serial
int int_read_serial_int() {
    while (Serial.available() == 0) {
        // Pausa para no saturar el loop
        #if defined(ESP32)
        vTaskDelay(50 / portTICK_PERIOD_MS);
        #else
        delay(50);
        #endif
    }
    return Serial.parseInt();
}

// ==========================
// Funciones Públicas (llamadas desde main)
// ==========================

/**
 * @brief Inicia el hardware de los POTs y guía el entrenamiento
 * Esta función es BLOQUEANTE y se llama 1 vez en setup()
 */
void PERCEPTRON_Init_Training() {
    // 1. Inicializar pines ADC
    ADC_Init(POT1_PIN);
    ADC_Init(POT2_PIN);
    ADC_Init(POT3_PIN);
    ADC_Init(POT4_PIN);
    ADC_Init(POT5_PIN);

    PRINT_Mensaje("----- PERCEPTRON SIMPLE BINARIO -----");
    PRINT_Mensaje("N_DIMENSIONS = 5 (Fijo)");

    // 2. Inicializar pesos aleatorios
    srand(0); // Semilla fija para reproducibilidad
    for (int i = 0; i < N_DIMENSIONS + 1; i++) {
        w[i] = (float)rand() / (float)RAND_MAX;
    }

    // 3. Generar tabla de verdad
    int_generate_truth_table();

    // 4. Pedir al usuario la función a aprender
    PRINT_Mensaje("Seleccione la función (0=AND, 1=OR, 2=Personalizada): ");
    int sel = int_read_serial_int();
    Serial.println(sel); // Echo

    switch (sel) {
        case 0: // AND
            PRINT_Mensaje("Entrenando para función AND...");
            for (int i = 0; i < N_ROWS; i++) {
                y[i] = 1;
                for (int k = 0; k < N_DIMENSIONS; k++) {
                    if (X[i][k] == 0) y[i] = 0;
                }
            }
            break;

        case 1: // OR
            PRINT_Mensaje("Entrenando para función OR...");
            for (int i = 0; i < N_ROWS; i++) {
                y[i] = 0;
                for (int k = 0; k < N_DIMENSIONS; k++) {
                    if (X[i][k] == 1) y[i] = 1;
                }
            }
            break;

        case 2: // Personalizada
            PRINT_Mensaje("--- Ingrese salidas deseadas (0 o 1) ---");
            for (int i = 0; i < N_ROWS; i++) {
                // Reemplazo de: Serial.printf("[%2d] Entrada: ", i);
                Serial.print("[");
                if (i < 10) Serial.print(" "); // Padding manual
                Serial.print(i);
                Serial.print("] Entrada: ");

                for (int k = 0; k < N_DIMENSIONS; k++) {
                    // Reemplazo de: Serial.printf("%d ", X[i][k]);
                    Serial.print(X[i][k]);
                    Serial.print(" ");
                }
                Serial.print("=> salida deseada: ");
                y[i] = int_read_serial_int();
                Serial.println(y[i]); // Echo
            }
            break;
        default:
            PRINT_Mensaje("Opción inválida. Reiniciando...");
            delay(1000);
            #if defined(ESP32)
            esp_restart();
            #else
            asm volatile ("  jmp 0"); // Soft reset para AVR (Uno)
            #endif
    }
    
    // 5. Entrenamiento
    PRINT_Mensaje("Entrenando perceptrón...");
    int it_max = 500, j = 0;
    float n = 0.5f; // Tasa de aprendizaje
    float Error[N_ROWS];
    
    do {
        float totalError = 0.0f;
        j++;
        for (int i = 0; i < N_ROWS; i++) {
            float r = 0.0f;
            for (int k = 0; k < N_DIMENSIONS; k++)
                r += w[k] * (float)X[i][k];
            r += w[N_DIMENSIONS]; // bias
            
            int out = int_salidaBin(r);
            Error[i] = (float)y[i] - (float)out;
            totalError += fabsf(Error[i]);

            // Regla de actualización de pesos
            for (int k = 0; k < N_DIMENSIONS; k++)
                w[k] += n * Error[i] * (float)X[i][k];
            w[N_DIMENSIONS] += n * Error[i];
        }
        if (totalError == 0.0f) break;
    } while (j < it_max);

    // 6. Resultados
    PRINT_Perceptron_Weights(w);
    // Reemplazo de: Serial.printf("Iteraciones: %d\n", j);
    Serial.print("Iteraciones: ");
    Serial.println(j);

    g_trained = true;
    PRINT_Mensaje("===== Entrenamiento Completo =====");
}


/**
 * @brief Lee los 5 POTs, los convierte a binario y ejecuta el modelo.
 */
void PERCEPTRON_Run_Update() {
    if (!g_trained) return; // No correr si no se ha entrenado

    // 1. Leer potenciómetros y convertir a 0 o 1
    g_current_inputs[0] = (analogRead(POT1_PIN) >= ADC_THRESHOLD) ? 1 : 0;
    g_current_inputs[1] = (analogRead(POT2_PIN) >= ADC_THRESHOLD) ? 1 : 0;
    g_current_inputs[2] = (analogRead(POT3_PIN) >= ADC_THRESHOLD) ? 1 : 0;
    g_current_inputs[3] = (analogRead(POT4_PIN) >= ADC_THRESHOLD) ? 1 : 0;
    g_current_inputs[4] = (analogRead(POT5_PIN) >= ADC_THRESHOLD) ? 1 : 0;

    // 2. Correr el modelo con las entradas actuales
    g_perceptron_output = int_run_model(g_current_inputs);

    // 3. Imprimir estado actual
    PRINT_Perceptron_Status(g_current_inputs, g_perceptron_output);
}

/**
 * @brief Devuelve la última salida calculada por el perceptrón.
 */
int PERCEPTRON_Get_Output() {
    return g_perceptron_output;
}