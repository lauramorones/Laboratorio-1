#include "bsp/BSP.h"

extern bool requestRetrain;
extern bool hasRunAfterTraining;

bool systemOn = false;
int currentMode = MODE_OFF;


// ====================== ESP32 (FreeRTOS) ======================
#if defined(ESP32)

void TaskBlink(void *pvParameters) {
  LED_PERCEPTRON_Init();
  LED_MODE_Init();

  while (1) {

    switch (currentMode) {

      case MODE_OFF:
        LED_MODE_Off();
        break;

      case MODE_LOWPOWER:
        LED_MODE_On();
        vTaskDelay(300 / portTICK_PERIOD_MS);
        LED_MODE_Off();
        vTaskDelay(300 / portTICK_PERIOD_MS);
        break;

      case MODE_RUN:
        LED_MODE_On();  // LED de sistema fijo encendido
        break;
    }

    // LED del perceptrón (refleja salida)
    if (PERCEPTRON_Get_Output() == 1)
        LED_PERCEPTRON_On();
    else
        LED_PERCEPTRON_Off();

    vTaskDelay(40 / portTICK_PERIOD_MS);
  }
}


void TaskPerceptron(void *pvParameters) {
  while (1) {

    if (requestRetrain) {
      requestRetrain = false;
      systemOn = false;
      currentMode = MODE_OFF;

      Serial.println("Reiniciando entrenamiento...");
      PERCEPTRON_Init_Training();
      hasRunAfterTraining = false;

      Serial.println("Presione el botón para iniciar.");
    }

    if (systemOn) {
      currentMode = MODE_RUN;
      PERCEPTRON_Run_Update();
    } else {
      currentMode = MODE_OFF;
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void TaskButton(void *pvParameters) {
  Button_Init();
  while (1) {
    Button_Update();
    vTaskDelay(15 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(9600);
  delay(300);

  LED_PERCEPTRON_Init();
  LED_MODE_Init();

  Serial.println("======================================");
  Serial.println("   Laboratorio Perceptrón Embebido    ");
  Serial.println("======================================");

  PERCEPTRON_Init_Training();
  hasRunAfterTraining = false;

  Serial.println("Presione el botón para iniciar.");

  xTaskCreate(TaskBlink, "LED", 2048, NULL, 1, NULL);
  xTaskCreate(TaskPerceptron, "Percep", 4096, NULL, 1, NULL);
  xTaskCreate(TaskButton, "Button", 2048, NULL, 2, NULL);
}

void loop() {}


// ====================== ARDUINO UNO ======================
#elif defined(ARDUINO_UNO)

unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(9600);
  delay(200);

  LED_PERCEPTRON_Init();
  LED_MODE_Init();
  Button_Init();

  Serial.println("======================================");
  Serial.println("   Laboratorio Perceptrón Embebido    ");
  Serial.println("======================================");

  PERCEPTRON_Init_Training();
  hasRunAfterTraining = false;

  Serial.println("Presione el botón para iniciar.");
}

void loop() {

  Button_Update();

  if (requestRetrain) {
    requestRetrain = false;
    systemOn = false;
    currentMode = MODE_OFF;

    Serial.println("Reiniciando entrenamiento...");
    PERCEPTRON_Init_Training();
    hasRunAfterTraining = false;

    Serial.println("Presione el botón para iniciar.");
  }

  switch (currentMode) {

    case MODE_OFF:
      LED_MODE_Off();
      break;

    case MODE_LOWPOWER:
      LED_MODE_On();
      delay(300);
      LED_MODE_Off();
      delay(300);
      break;

    case MODE_RUN:
      LED_MODE_On();   // LED del sistema ENCENDIDO
      break;
  }

  // LED del perceptrón
  if (PERCEPTRON_Get_Output() == 1)
      LED_PERCEPTRON_On();
  else
      LED_PERCEPTRON_Off();

  if (systemOn) {
    unsigned long now = millis();
    if (now - lastUpdate >= 1000) {
      lastUpdate = now;
      currentMode = MODE_RUN;
      PERCEPTRON_Run_Update();
    }
  } else {
    currentMode = MODE_OFF;
  }
}

#endif
