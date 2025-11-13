#include "bsp/BSP.h"
#include "perceptron/perceptron.h"

// ==========================
//    VARIABLES GLOBALES
// ==========================
int entradaRT[5];

// ==========================
//   TAREA — SOLO ESP32
// ==========================
#if defined(ESP32)
void TaskPerceptron(void *pvParams) {
  while (1) {
    Read_Pots(entradaRT);
    int out = Perceptron_Predict(entradaRT);

    if (out == 1) LED_ML_On();
    else LED_ML_Off();

    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}
#endif

// ==========================
//        SETUP
// ==========================
void setup() {
  Serial.begin(9600);
  delay(200);

  Serial.println("====================================");
  Serial.println("  SISTEMA PERCEPTRON UNIFICADO");
  Serial.println("====================================");

  // ==== Inicializar hardware común ====
  LED_Init();
  ADC_Init(A0); 
  ADC_Init(A1);
  ADC_Init(A2);
  ADC_Init(A3);
  ADC_Init(A4);

  // ==== Detectar potenciómetros ====
  Detect_Pots();

  if (N_inputs == 0) {
    Serial.println("ERROR: No se detectó ningún potenciómetro.");
    while (1);
  }

  int filas = 1 << N_inputs;

  // ==== Memoria dinámica estática ====
  int X[MAX_ROWS][MAX_INPUTS];
  int y[MAX_ROWS];

  // ==== Generar tabla de verdad X ====
  Generate_TruthTable(X);

  // ==== Pedir salida deseada ====
  Serial.println("\nIngrese un número decimal que representa la salida deseada:");
  Serial.print("(0 a ");
  Serial.print((1UL << N_inputs) - 1);
  Serial.println("):");

  while (Serial.available() == 0);
  unsigned long salidaDeseada = Serial.parseInt();

  // Convertir a tabla de verdad y[] usando bits del número
  Apply_DesiredOutput(y, filas, salidaDeseada);

  // ==== Entrenar perceptrón ====
  Train_Perceptron(X, y, filas);

  Serial.println("\nPerceptrón listo. Comenzando predicción en tiempo real...\n");

  // =====================================================
  //       CAMINO DIFERENTE SEGÚN EL MICROCONTROLADOR
  // =====================================================
#if defined(ESP32)

  // ====== FREE RTOS ======
  xTaskCreate(TaskPerceptron, "Perceptron", 4096, NULL, 1, NULL);

#else

  // ==== MODO ARDUINO UNO ====
  // Nada especial aquí, loop() hará todo

#endif
}

// ==========================
//     LOOP — SOLO UNO
// ==========================
void loop() {

#if defined(ARDUINO_UNO)

  Read_Pots(entradaRT);
  int out = Perceptron_Predict(entradaRT);

  if (out == 1)
    LED_On();
  else
    LED_Off();

  delay(200);

#else

  // ESP32 no usa loop()
  vTaskDelay(100 / portTICK_PERIOD_MS);

#endif
}
