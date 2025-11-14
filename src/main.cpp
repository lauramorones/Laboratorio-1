#include "bsp/BSP.h"

// ======== ESP32 con FreeRTOS ========
#if defined(ESP32)

/**
 * @brief Tarea del LED (ESP32).
 * Refleja la SALIDA (0 o 1) del Perceptrón.
 */
void TaskBlink(void *pvParameters) {
  LED_Init();
  while (1) {
    switch (currentMode) {
      case MODE_OFF:
        LED_Off();
        vTaskDelay(500 / portTICK_PERIOD_MS); // Pausa
        break;
      
      case MODE_RUN:
        if (PERCEPTRON_Get_Output() == 1) {
            LED_On();
        } else {
            LED_Off();
        }
        vTaskDelay(50 / portTICK_PERIOD_MS); // Refresca rápido
        break;
    }
  }
}

/**
 * @brief Tarea del Perceptrón (ESP32).
 * Lee los POTs y corre el modelo si el sistema está ON.
 */
void TaskPerceptron(void *pvParameters) {
  while (1) {
    if (systemOn) {
      PERCEPTRON_Run_Update(); // Lee pots, corre modelo, imprime
      currentMode = MODE_RUN;
    } else {
      currentMode = MODE_OFF;
    }
    // Actualiza la lectura 1 vez por segundo
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
  }
}

/**
 * @brief Tarea del Botón (ESP32).
 */
void TaskButton(void *pvParameters) {
  Button_Init();
  while (1) {
    Button_Update();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("======================================");
  Serial.println("   Laboratorio Perceptrón Embebido    ");
  Serial.println("======================================");

  // --- ENTRENAMIENTO (Bloqueante) ---
  PERCEPTRON_Init_Training();

  Serial.println("\n======================================");
  Serial.println(" Presione el boton para iniciar el sistema ");
  Serial.println("======================================");

  // --- TAREAS ---
  xTaskCreate(TaskBlink, "LED", 2048, NULL, 1, NULL);
  xTaskCreate(TaskPerceptron, "Perceptron", 4096, NULL, 1, NULL);
  xTaskCreate(TaskButton, "Button", 2048, NULL, 2, NULL);
}

void loop() {
  // FreeRTOS maneja todo
}

// ===== Versión Arduino UNO =====
#elif defined(ARDUINO_UNO)

unsigned long lastPerceptronUpdate = 0;
const long updateInterval = 1000; // 1 segundo

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("======================================");
  Serial.println("   Laboratorio Perceptrón Embebido    ");
  Serial.println("======================================");
  
  // Inits
  LED_Init();
  Button_Init();

  // --- ENTRENAMIENTO (Bloqueante) ---
  PERCEPTRON_Init_Training();

  Serial.println("\n======================================");
  Serial.println(" Presione el boton para iniciar el sistema ");
  Serial.println("======================================");
}

void loop() {
  // 1. Actualiza el botón siempre
  Button_Update(); // Esto actualiza 'systemOn' y 'currentMode'

  // 2. Lógica del Perceptrón (solo si está encendido)
  if (systemOn) {
    currentMode = MODE_RUN;
    unsigned long now = millis();
    
    // Ejecuta la lectura de POTs cada 'updateInterval'
    if (now - lastPerceptronUpdate >= updateInterval) {
      lastPerceptronUpdate = now;
      PERCEPTRON_Run_Update(); // Lee pots, corre modelo, imprime
    }
  } else {
      currentMode = MODE_OFF;
  }

  // 3. Control del LED
  switch (currentMode) {
    case MODE_OFF:
      LED_Off();
      break;
    case MODE_RUN:
      // El LED refleja la salida del perceptrón
      if (PERCEPTRON_Get_Output() == 1) {
        LED_On();
      } else {
        LED_Off();
      }
      break;
  }
}
#endif