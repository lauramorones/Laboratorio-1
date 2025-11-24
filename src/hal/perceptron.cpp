#include "bsp/BSP.h"

// =====================================================
//  PROTOTIPOS NECESARIOS (EVITAN LOS ERRORES DE COMPILACIÓN)
// =====================================================
void init_weights();
void tabla_verdad();
int int_read_serial_int();
void build_outputs(int sel);
void entrenamiento_LMS();


// ==========================
// Variables globales
// ==========================
int N_inputs = 0;

// Pesos: hasta 5 entradas (pots) + sesgo
float w[N_DIMENSIONS + 1];

int   X[N_ROWS][N_DIMENSIONS];
int   y_bin[N_ROWS];

int   entradas_actuales[N_DIMENSIONS];
int   salida_actual = 0;
bool  train_listo = false;

const int potPins[N_DIMENSIONS]    = { POT1_PIN, POT2_PIN, POT3_PIN, POT4_PIN, POT5_PIN };
const int potDetPins[N_DIMENSIONS] = { POT1_DET_PIN, POT2_DET_PIN, POT3_DET_PIN, POT4_DET_PIN, POT5_DET_PIN };

int activePotPins[N_DIMENSIONS];

int salida_select = -1;


// =========================================================
// Funciones auxiliares del perceptrón
// =========================================================

int bin_step(float x) {
    return (x >= 0.0f ? 1 : 0);
}

float net_input(int *inputs) {
    float r = 0.0f;
    for (int k = 0; k < N_inputs; k++)
        r += w[k] * (float)inputs[k];
    r += w[N_inputs]; // bias
    return r;
}

int model_predict(int *inputs) {
    return bin_step(net_input(inputs));
}


// =========================================================
// Generar tabla de verdad
// =========================================================
void tabla_verdad() {
    int rows = (int)pow(2, N_inputs);
    for (int i = 0; i < rows; i++) {
        for (int k = 0; k < N_inputs; k++) {
            X[i][k] = (i >> (N_inputs - k - 1)) & 1;
        }
    }
}


// =========================================================
// Leer entero por Serial
// =========================================================
int int_read_serial_int() {
    String s = "";

    while (true) {
        if (Serial.available()) {
            char c = Serial.read();

            if (c == '\n' || c == '\r') {
                if (s.length() > 0) {
                    return s.toInt();
                }
            } else {
                s += c;
            }
        }

        #if defined(ESP32)
        vTaskDelay(10 / portTICK_PERIOD_MS);
        #else
        delay(10);
        #endif
    }
}


// =========================================================
// Detección dinámica de potenciómetros
// =========================================================
int DETECT_Pots() {
    N_inputs = 0;

    for (int i = 0; i < N_DIMENSIONS; i++) {
        GPIO_PullUp(potDetPins[i]);
        delay(2);

        int val = GPIO_Read(potDetPins[i]);

        if (val == LOW) {
            activePotPins[N_inputs] = potPins[i];
            N_inputs++;
        }
    }

    return N_inputs;
}


// =========================================================
// Inicializar pesos aleatorios
// =========================================================
void init_weights() {
    srand(0);
    for (int i = 0; i < N_DIMENSIONS + 1; i++) {
        float r = ((float)rand() / RAND_MAX);
        w[i] = (r - 0.5f) * 0.2f;
    }
}


// =========================================================
// Entrenamiento LMS (igual que versión previa)
// =========================================================
void entrenamiento_LMS() {

    PRINT_Mensaje("Entrenando con LMS ...");

    int rows = (int)pow(2, N_inputs);

    float MinError = 0.01f;
    float n = 0.5f;
    int it_max = 500;
    int j = 0;

    float Error[N_ROWS];
    for (int i = 0; i < rows; i++) Error[i] = 1.0f;

    while (true) {

        bool error_grande = false;
        for (int i = 0; i < rows; i++) {
            if (fabs(Error[i]) >= MinError) {
                error_grande = true;
                break;
            }
        }

        if (!error_grande) break;
        if (j >= it_max) break;

        j++;

        for (int i = 0; i < rows; i++) {

            float r = 0.0f;
            for (int k = 0; k < N_inputs; k++)
                r += w[k] * X[i][k];
            r += w[N_inputs];

            int out = (r >= 0.0f ? 1 : 0);

            Error[i] = (float)(y_bin[i] - out);

            for (int k = 0; k < N_inputs; k++)
                w[k] += n * Error[i] * X[i][k];

            w[N_inputs] += n * Error[i];
        }
    }

    Serial.print("Iteraciones finales: ");
    Serial.println(j);

    if (j >= it_max) {
        Serial.println("********************************************");
        Serial.println("   EL PERCEPTRON NO PUDO APRENDER LA FUNCION");
        Serial.println("********************************************");
        Serial.println("PROC_NO_CONVERGE");
    }
    else {
        Serial.println("Modelo entrenado correctamente.");
        Serial.println("PROC_TRAIN_OK");
    }
}


// =========================================================
// Construir tabla de salidas
// =========================================================
void build_outputs(int sel) {

    int rows = 1 << N_inputs;
    salida_select = sel;

    switch (sel) {

        case 0: { // AND
            PRINT_Mensaje("Funcion AND seleccionada");
            for (int i = 0; i < rows; i++) {
                int salida = 1;
                for (int k = 0; k < N_inputs; k++)
                    if (X[i][k] == 0) salida = 0;
                y_bin[i] = salida;
            }
            break;
        }

        case 1: { // OR
            PRINT_Mensaje("Funcion OR seleccionada");
            for (int i = 0; i < rows; i++) {
                int salida = 0;
                for (int k = 0; k < N_inputs; k++)
                    if (X[i][k] == 1) salida = 1;
                y_bin[i] = salida;
            }
            break;
        }

        case 2: { // tabla personalizada decimal
            PRINT_Mensaje("Funcion DECIMAL personalizada seleccionada");

            unsigned long dec = int_read_serial_int();
            Serial.print("Número decimal recibido: ");
            Serial.println(dec);

            for (int i = 0; i < rows; i++) {
                int bitIndex = rows - 1 - i;
                y_bin[i] = (dec >> bitIndex) & 1U;
            }

            Serial.println("Tabla personalizada:");
            for (int i = 0; i < rows; i++) {
                Serial.print("X[");
                for (int k = 0; k < N_inputs; k++) Serial.print(X[i][k]);
                Serial.print("] => ");
                Serial.println(y_bin[i]);
            }
            break;
        }

        default:
            PRINT_Mensaje("Opcion invalida");
            delay(1000);

            #if defined(ESP32)
            esp_restart();
            #else
            asm volatile("jmp 0");
            #endif
    }
}


// =========================================================
// ENTRENAMIENTO COMPLETO (con LOW POWER real)
// =========================================================
void PERCEPTRON_Init_Training() {

    ADC_Init(POT1_PIN);
    ADC_Init(POT2_PIN);
    ADC_Init(POT3_PIN);
    ADC_Init(POT4_PIN);
    ADC_Init(POT5_PIN);

    PRINT_Mensaje("===== INICIANDO ENTRENAMIENTO =====");
    PRINT_Mensaje("Detectando potenciometros...");

    while (true) {
        if (DETECT_Pots() > 0) break;

        PRINT_Mensaje("0 pots detectados -> LOW POWER");

        currentMode = MODE_LOWPOWER;

        // LED del sistema parpadea
        LED_MODE_On();
        delay(300);
        LED_MODE_Off();
        delay(300);
    }

    Serial.print("Potenciómetros detectados: ");
    Serial.println(N_inputs);

    init_weights();
    tabla_verdad();

    PRINT_Mensaje("Seleccione la función:");
    PRINT_Mensaje("0 = AND");
    PRINT_Mensaje("1 = OR");
    PRINT_Mensaje("2 = Personalizada DECIMAL");

    int sel = int_read_serial_int();
    build_outputs(sel);

    entrenamiento_LMS();

    PRINT_Perceptron_Weights(w);

    PRINT_Mensaje("===== ENTRENAMIENTO COMPLETO =====");

    train_listo = true;
}

// =========================================================
// RUN
// =========================================================
void PERCEPTRON_Run_Update() {

    if (!train_listo || N_inputs <= 0) return;

    for (int i = 0; i < N_inputs; i++) {
        int raw = analogRead(activePotPins[i]);
        entradas_actuales[i] = (raw >= ADC_THRESHOLD) ? 1 : 0;
    }

    salida_actual = model_predict(entradas_actuales);

    PRINT_Perceptron_Status(entradas_actuales, salida_actual);
}

int PERCEPTRON_Get_Output() {
    return salida_actual;
}
