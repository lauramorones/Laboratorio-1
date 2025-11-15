#include "bsp/BSP.h"

// ==========================
// Variables globales del Perceptrón
// ==========================

// Número real de potenciómetros detectados
int N_inputs = 0;

// Pesos: hasta N_DIMENSIONS entradas + 1 bias
float w[N_DIMENSIONS + 1];

// Matriz de entradas (tabla de verdad) y salidas deseadas
int   X[N_ROWS][N_DIMENSIONS];
int   y[N_ROWS];

// Entradas actuales de los POTs (en tiempo real)
int   g_current_inputs[N_DIMENSIONS];
// Salida actual del modelo
int   g_perceptron_output = 0;
// Flag para saber si el modelo está listo
bool  g_trained = false;

// Pines de potenciómetros (ADC) y pines auxiliares de detección
const int potPins[N_DIMENSIONS] = {
    POT1_PIN,
    POT2_PIN,
    POT3_PIN,
    POT4_PIN,
    POT5_PIN
};

const int potDetPins[N_DIMENSIONS] = {
    POT1_DET_PIN,
    POT2_DET_PIN,
    POT3_DET_PIN,
    POT4_DET_PIN,
    POT5_DET_PIN
};

// Lista de pines activos (solo los que realmente tienen pot conectado)
int activePotPins[N_DIMENSIONS];

// ==========================
// Funciones internas
// ==========================

// Función de activación binaria
int int_salidaBin(float x) {
    return (x >= 0) ? 1 : 0;
}

// Ejecuta el modelo con los pesos ya entrenados
// Usa solo N_inputs entradas y el bias en w[N_inputs]
int int_run_model(int* inputs) {
    float r = 0.0f;
    for (int k = 0; k < N_inputs; k++) {
        r += w[k] * (float)inputs[k];
    }
    // Bias
    r += w[N_inputs];
    return int_salidaBin(r);
}

// Genera la tabla de verdad (entradas X) para N_inputs
void int_generate_truth_table() {
    int rows = 1 << N_inputs;

    for (int i = 0; i < rows; i++) {
        for (int k = 0; k < N_inputs; k++) {
            // Bit k-ésimo sobre N_inputs
            X[i][k] = (i >> (N_inputs - k - 1)) & 1;
        }
    }
}

// Espera un input numérico desde el Monitor Serial
int int_read_serial_int() {
    while (Serial.available() == 0) {
        // Pausa para no saturar el loop
        #if defined(ESP32)
        vTaskDelay(40 / portTICK_PERIOD_MS);
        #else
        delay(40);
        #endif
    }
    return Serial.parseInt();
}

// ==========================
// Detección de potenciómetros con pines auxiliares
// ==========================
//
// Cada potenciómetro tiene un pin auxiliar de detección (POTx_DET_PIN).
// Se asume que cuando el pot está conectado, este pin se va a GND (activo en LOW).
// Usamos INPUT_PULLUP para leerlo:
//   - LOW  -> pot conectado
//   - HIGH -> sin pot / abierto
//
int DETECT_Pots() {
    N_inputs = 0;

    for (int i = 0; i < N_DIMENSIONS; i++) {
        pinMode(potDetPins[i], INPUT_PULLUP);
        delay(2); // pequeño tiempo para estabilizar

        int val = digitalRead(potDetPins[i]);

        if (val == LOW) {
            // Este pot se considera conectado
            activePotPins[N_inputs] = potPins[i];
            N_inputs++;
        }
    }

    return N_inputs;
}

// ==========================
// Entrenamiento
// ==========================

void PERCEPTRON_Init_Training() {
    // 1. Inicializar pines ADC (para todos los potenciómetros posibles)
    ADC_Init(POT1_PIN);
    ADC_Init(POT2_PIN);
    ADC_Init(POT3_PIN);
    ADC_Init(POT4_PIN);
    ADC_Init(POT5_PIN);

    PRINT_Mensaje("===== PERCEPTRON SIMPLE EMBEBIDO =====");
    PRINT_Mensaje("Detectando potenciómetros (pines auxiliares)...");

    // 2. Esperar hasta que detectemos al menos un pot
    while (true) {
        int detected = DETECT_Pots();

        if (detected > 0) {
            Serial.print("Potenciómetros detectados: ");
            Serial.println(detected);
            break;
        }

        PRINT_Mensaje("0 potenciometros detectados -> MODO LOW POWER");
        PRINT_Mensaje("Conecte al menos 1 potenciómetro a los pines auxiliares...");

        LED_Off();

        #if defined(ESP32)
        vTaskDelay(1200 / portTICK_PERIOD_MS);
        #else
        delay(1200);
        #endif
    }

    Serial.print("Entradas activas (potenciómetros conectados): ");
    Serial.println(N_inputs);

    // 3. Inicializar pesos aleatorios (para todas las posiciones disponibles)
    srand(0); // Semilla fija para reproducibilidad
    for (int i = 0; i < N_DIMENSIONS + 1; i++) {
        w[i] = (float)rand() / (float)RAND_MAX;
    }

    // 4. Generar tabla de verdad SOLO para N_inputs
    int_generate_truth_table();
    int rows = 1 << N_inputs;

    // 5. Pedir al usuario la función a aprender
    Serial.println();
    PRINT_Mensaje("Seleccione la función:");
    PRINT_Mensaje("0 = AND");
    PRINT_Mensaje("1 = OR");
    PRINT_Mensaje("2 = Personalizada DECIMAL");

    int sel = int_read_serial_int();
    Serial.println(sel);

    switch (sel) {
        case 0: { // AND
            PRINT_Mensaje("Entrenando para función AND...");
            for (int i = 0; i < rows; i++) {
                y[i] = 1;
                for (int k = 0; k < N_inputs; k++) {
                    if (X[i][k] == 0) {
                        y[i] = 0;
                        break;
                    }
                }
            }
            break;
        }

        case 1: { // OR
            PRINT_Mensaje("Entrenando para función OR...");
            for (int i = 0; i < rows; i++) {
                y[i] = 0;
                for (int k = 0; k < N_inputs; k++) {
                    if (X[i][k] == 1) {
                        y[i] = 1;
                        break;
                    }
                }
            }
            break;
        }

        case 2: { // Personalizada por número decimal
            PRINT_Mensaje("Modo Personalizado DECIMAL");
            PRINT_Mensaje("Ingrese un solo numero entero decimal");
            PRINT_Mensaje("que represente la tabla de verdad completa.");

            int rows_local = rows;

            Serial.print("Numero de combinaciones (filas): ");
            Serial.println(rows_local);
            PRINT_Mensaje("Nota: se usan los bits menos significativos del numero.");

            Serial.print("Ingrese el numero decimal: ");
            unsigned long dec = (unsigned long) int_read_serial_int();
            Serial.println(dec);

            // Convertir decimal -> bits -> salidas Y[]
            for (int i = 0; i < rows_local; i++) {
                int bit_index = rows_local - 1 - i; // MSB primero
                y[i] = ( (dec >> bit_index) & 1UL );
            }

            // Imprimir tabla para verificación
            Serial.println("Tabla personalizada:");
            for (int i = 0; i < rows_local; i++) {
                Serial.print("Entrada [");
                for (int k = 0; k < N_inputs; k++) {
                    Serial.print(X[i][k]);
                }
                Serial.print("] => ");
                Serial.println(y[i]);
            }
            break;
        }

        default:
            PRINT_Mensaje("Opción inválida. Reiniciando...");
            delay(1000);
            #if defined(ESP32)
            esp_restart();
            #else
            asm volatile ("  jmp 0"); // Soft reset para AVR (Uno)
            #endif
    }

    // 6. Entrenamiento del perceptrón
    PRINT_Mensaje("Entrenando perceptrón...");
    int it_max = 500, j = 0;
    float n = 0.5f; // Tasa de aprendizaje
    float totalError;

    do {
        totalError = 0.0f;
        j++;
        for (int i = 0; i < rows; i++) {
            float r = 0.0f;
            for (int k = 0; k < N_inputs; k++) {
                r += w[k] * (float)X[i][k];
            }
            r += w[N_inputs]; // bias

            int out = int_salidaBin(r);
            float error = (float)y[i] - (float)out;
            totalError += fabsf(error);

            // Regla de actualización de pesos
            for (int k = 0; k < N_inputs; k++) {
                w[k] += n * error * (float)X[i][k];
            }
            w[N_inputs] += n * error;
        }
    } while (totalError != 0.0f && j < it_max);

    // 7. Resultados
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
    if (!g_trained || N_inputs <= 0) {
        return; // No correr si no se ha entrenado o no hay entradas
    }

    // 1. Leer potenciómetros activos y convertir a 0 o 1
    for (int i = 0; i < N_inputs; i++) {
        int raw = analogRead(activePotPins[i]);
        g_current_inputs[i] = (raw >= ADC_THRESHOLD) ? 1 : 0;
    }

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
