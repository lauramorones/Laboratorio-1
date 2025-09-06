#include "bsp/BSP.h"

// ======== ESP32 con FreeRTOS ========
#if defined(ESP32)

void TaskBlink(void *pvParameters) {
  LED_Init();
  while (1) {
    switch (currentMode) {
      case MODE_OFF:
        LED_Off();
        vTaskDelay(500 / portTICK_PERIOD_MS);  //si se necesita
        break;
      case MODE_LOWPOWER:
        LED_On();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        LED_Off();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        break;
      case MODE_RUN:
        LED_On();
        //vTaskDelay(500 / portTICK_PERIOD_MS);
        break;
    }
  }
}

void TaskSensores(void *pvParameters) {
  ADC_Init(TEMP_PIN);
  ADC_Init(HUM_PIN);
  ADC_Init(LUZ_PIN);
  Sensors_Init();
  while (1) {
    if (systemOn) {
      UpdateModeBySensors();
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}

void TaskButton(void *pvParameters) {
  Button_Init();
  while (1) {
    Button_Update();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("======================================");
  Serial.println(" Presione el boton para iniciar el sistema ");
  Serial.println("======================================");

  xTaskCreate(TaskBlink, "LED", 2048, NULL, 1, NULL);
  xTaskCreate(TaskSensores, "Sensores", 2048, NULL, 1, NULL);
  xTaskCreate(TaskButton, "Button", 2048, NULL, 2, NULL);
}

void loop() {
  // FreeRTOS maneja todo
}

// ===== Versión Arduino UNO  =====
#elif defined(ARDUINO_UNO)
unsigned long lastBlink = 0;
unsigned long lastSens = 0;

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("======================================");
  Serial.println(" Presione el boton para iniciar el sistema ");
  Serial.println("======================================");

  LED_Init();
  Button_Init();
  ADC_Init(TEMP_PIN);
  ADC_Init(HUM_PIN);
  ADC_Init(LUZ_PIN);
  Sensors_Init();
}

void loop() {
  unsigned long now = millis();

  // Actualiza el botón
  Button_Update();

  // Leer sensores cada 3 segundos
  if (now - lastSens > 3000) {
    lastSens = now;
    if (systemOn) {
      UpdateModeBySensors();
    }
  }

  // Control del LED según modo
  switch (currentMode) {
    case MODE_OFF:
      LED_Off();
      break;
    case MODE_LOWPOWER:
      if (now - lastBlink >= 1000) { // 1 segundo
        lastBlink = now;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // invierte el LED
      }
      break;
    case MODE_RUN:
      LED_On(); // encendido fijo
      break;
  }
}
#endif
