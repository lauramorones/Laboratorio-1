#include "perceptron/perceptron.h"
#include "bsp/BSP.h"

int entradaRT[5];

#if defined(ESP32)
void TaskPerceptron(void *pvParams) {
    while (1) {

        if (!systemOn || currentMode != MODE_RUN) {
            LED_ML_Off();
            vTaskDelay(80 / portTICK_PERIOD_MS);
            continue;
        }

        Read_Pots(entradaRT);
        int out = Perceptron_Predict(entradaRT);

        if (out == 1) LED_ML_On();
        else LED_ML_Off();

        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
#endif

void setup() {
    Serial.begin(9600);
    delay(300);

    LED_Init();
    Button_Init();

    ADC_Init(POT1_PIN);
    ADC_Init(POT2_PIN);
    ADC_Init(POT3_PIN);
    ADC_Init(POT4_PIN);
    ADC_Init(POT5_PIN);

    Serial.println("\n=== SISTEMA PERCEPTRÓN ===");
    Serial.println("Presiona botón para encender...");
}

void loop() {

    Button_Update();

    // ========== OFF ==========
    if (!systemOn || currentMode == MODE_OFF) {
        LED_Off();
        LED_ML_Off();
        delay(100);
        return;
    }

    // ========== Detectar Pots ==========
    N_inputs = Detect_Pots();

    // ========== LOW-POWER ==========
    if (currentMode == MODE_LOWPOWER) {
        LED_Off();
        LED_ML_Off();
        Serial.println("LOW POWER: Conecta potenciómetros...");
        delay(300);
        return;
    }

    // ========== RUN ==========
#if defined(ARDUINO_UNO)

    static bool trained = false;

    if (!trained) {
        trained = true;

        int filas = 1 << N_inputs;
        int X[MAX_ROWS][MAX_INPUTS];
        int y[MAX_ROWS];

        Generate_TruthTable(X);

        Serial.print("Salida decimal (0 a ");
        Serial.print(filas - 1);
        Serial.println("): ");

        while (Serial.available() == 0);
        unsigned long salida = Serial.parseInt();

        Apply_DesiredOutput(y, filas, salida);

        Train_Perceptron(X, y, filas);

        Serial.println("Perceptrón entrenado\n");
    }

    Read_Pots(entradaRT);
    int out = Perceptron_Predict(entradaRT);

    if (out == 1) LED_On();
    else LED_Off();

    delay(200);

#else  // ESP32

    static bool started = false;

    if (!started) {
        started = true;

        int filas = 1 << N_inputs;
        int X[MAX_ROWS][MAX_INPUTS];
        int y[MAX_ROWS];

        Generate_TruthTable(X);

        Serial.print("Salida decimal (0 a ");
        Serial.print(filas - 1);
        Serial.println("): ");

        while (Serial.available() == 0);
        unsigned long salida = Serial.parseInt();

        Apply_DesiredOutput(y, filas, salida);

        Train_Perceptron(X, y, filas);

        xTaskCreate(TaskPerceptron,
                    "TaskPerceptron",
                    4096,
                    NULL,
                    1,
                    NULL);
    }

    vTaskDelay(20 / portTICK_PERIOD_MS);

#endif
}
