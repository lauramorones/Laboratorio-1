#include "bsp/BSP.h"

// ==========================
// Variables globales
// ==========================

int N_inputs = 0;

// Pesos: hasta 5 entradas (pots) + sesgo
float w[N_DIMENSIONS + 1];

// Entradas X[][], salida lógica y_bin[]
int   X[N_ROWS][N_DIMENSIONS];
int   y_bin[N_ROWS];  
//float t[N_ROWS];       // Ya no se usa en este método, pero se mantiene por compatibilidad

int   entradas_actuales[N_DIMENSIONS];  //De los pots
int   salida_actual = 0;
bool  train_listo = false;

const int potPins[N_DIMENSIONS]     = { POT1_PIN, POT2_PIN, POT3_PIN, POT4_PIN, POT5_PIN };
const int potDetPins[N_DIMENSIONS]  = { POT1_DET_PIN, POT2_DET_PIN, POT3_DET_PIN, POT4_DET_PIN, POT5_DET_PIN };

int activePotPins[N_DIMENSIONS];

// Para recordar la función seleccionada
int salida_select = -1;

//Función de activacion binaria, da 1 o 0
int bin_step(float x) {
    return (x >= 0.0f ? 1 : 0);
}

//Calcula solo el valor lineal antes de pasar por bin_step. Suma ponderada de todas las entradas
float net_input (int *inputs) {
    float r = 0.0f;
    for (int k = 0; k < N_inputs; k++) {
        r += w[k] * (float)inputs[k];
    }
    r += w[N_inputs]; // bias
    return r;
}

//función para predicción del modelo
int model_predict(int *inputs) {
    float r = net_input (inputs);
    return bin_step(r);
}

// Generar tabla de verdad
void tabla_verdad () {
    int rows = (int)pow(2, N_inputs);   // 2^N_inputs

    for (int i = 0; i < rows; i++) {
        for (int k = 0; k < N_inputs; k++) {

            // Extraer el bit correspondiente
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
        GPIO_PullUp(potDetPins[i]);
        delay(2);  // pequeño settle

        int val = GPIO_Read(potDetPins[i]);
        if (val == LOW) {
            activePotPins[N_inputs] = potPins[i];
            N_inputs++;
        }
    }
    return N_inputs;
}



// Inicializar pesos aleatorios pequeños
void init_weights() {
    srand(0);
    for (int i = 0; i < N_DIMENSIONS + 1; i++) {
        float r = ((float)rand() / RAND_MAX);
        w[i] = (r - 0.5f) * 0.2f;
    }
}



// ==========================
// ENTRENAMIENTO MATLAB STYLE
// ==========================
// LMS con hardlim(), idéntico a MATLAB
// r = w·x + bias
// out = hardlim(r)
// error = y - out
// w = w + n * error * x
// bias = bias + n * error
// ==========================

void train_LMS_matlab_style() {

    PRINT_Mensaje("Entrenando con LMS estilo MATLAB (Juan Ramirez)...");

    int rows = 1 << N_inputs;

    float MinError = 0.01f;
    float n = 0.5f;
    int it_max = 500;
    int j = 0;

    float Error[N_ROWS];

    for (int i = 0; i < rows; i++)
        Error[i] = 1.0f;


    while (true) {

        bool stillBig = false;
        for (int i = 0; i < rows; i++) {
            if (fabs(Error[i]) >= MinError) {
                stillBig = true;
                break;
            }
        }

        if (!stillBig) break;
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
}




// ==========================
// Construir salidas deseadas (AND, OR, Decimal)
// ==========================

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

        case 2: { // Decimal personalizado
            PRINT_Mensaje("Funcion DECIMAL personalizada seleccionada");

            unsigned long dec = int_read_serial_int();
            Serial.print("Número decimal recibido: ");
            Serial.println(dec);

            for (int i = 0; i < rows; i++) {
                int bitIndex = rows - 1 - i;
                int bit = (dec >> bitIndex) & 1U;
                y_bin[i] = bit;
            }

            Serial.println("Tabla personalizada (X => y):");
            for (int i = 0; i < rows; i++) {
                Serial.print("X[");
                for (int k = 0; k < N_inputs; k++) Serial.print(X[i][k]);
                Serial.print("] => ");
                Serial.println(y_bin[i]);
            }

            break;
        }

        default:
            PRINT_Mensaje("Opción inválida.");
            delay(1000);
            #if defined(ESP32)
            esp_restart();
            #else
            asm volatile("jmp 0");
            #endif
    }
}



// ==========================
// ENTRENAMIENTO COMPLETO
// ==========================

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
        LED_Off();

        #if defined(ESP32)
        vTaskDelay(1200 / portTICK_PERIOD_MS);
        #else
        delay(1200);
        #endif
    }

    Serial.print("Potenciómetros detectados: ");
    Serial.println(N_inputs);

    init_weights();

    tabla_verdad();

    PRINT_Mensaje("\nSeleccione la función:");
    PRINT_Mensaje("0 = AND");
    PRINT_Mensaje("1 = OR");
    PRINT_Mensaje("2 = Personalizada DECIMAL");

    int sel = int_read_serial_int();
    salida_select = sel;
    Serial.println(sel);

    build_outputs(sel);

    // Siempre usar el método MATLAB
    train_LMS_matlab_style();

    PRINT_Perceptron_Weights(w);
    PRINT_Mensaje("===== ENTRENAMIENTO COMPLETO =====");

    train_listo = true;
}



// ==========================
// RUN-TIME DEL MODELO
// ==========================

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
