#include "bsp/BSP.h"
#include "ml/ml_layer.h"

// ===============================
// Inicialización de sensores
// ===============================
void Sensors_Init() {
    GPIO_PullUp(TEMP_DET);
    GPIO_PullUp(HUM_DET);
    GPIO_PullUp(LUZ_DET);
}

// ===============================
// Verifica si un sensor está conectado
// ===============================
bool SensorConnected(int pinDet) {
    // El pin está en LOW cuando hay un sensor conectado
    return (digitalRead(pinDet) == LOW);
}

// ===============================
// Actualiza el modo del sistema según sensores y ML
// ===============================
void UpdateModeBySensors() {
    if (systemOn == 0) {
        currentMode = MODE_OFF;
        LED_Off();
        LED_ML_Off(); // apaga el LED ML también
        return;
    }

    // Si hay al menos un sensor conectado
    if (SensorConnected(TEMP_DET) || SensorConnected(HUM_DET) || SensorConnected(LUZ_DET)) {

        float temp = 0, hum = 0, luzVal = 0;

        // ===== Temperatura =====
        if (SensorConnected(TEMP_DET)) {
            temp = ADC_Read(TEMP_PIN);
            PRINT_Temp(temp);
        } else {
            PRINT_SensorNoAvailable("Temperatura");
        }

        // ===== Humedad =====
        if (SensorConnected(HUM_DET)) {
            hum = ADC_Read(HUM_PIN);
            PRINT_Humedad(hum);
        } else {
            PRINT_SensorNoAvailable("Humedad");
        }

        // ===== Luz =====
        if (SensorConnected(LUZ_DET)) {
            luzVal = ADC_Read(LUZ_PIN);
            PRINT_Luz(luzVal);
        } else {
            PRINT_SensorNoAvailable("Luz");
        }

        // ====== Capa de Machine Learning ======
        int decision = ML_Predict(temp, hum, luzVal);

        if (decision == 1) {
            LED_ML_On();   // LED exclusivo del modelo ML
            PRINT_Mensaje("ML: LED ON (Clase 1)");
        } else {
            LED_ML_Off();  // LED exclusivo del modelo ML
            PRINT_Mensaje("ML: LED OFF (Clase 0)");
        }

        // Cambia modo a RUN
        currentMode = MODE_RUN;
        PRINT_Mensaje("Sistema en modo RUN");

    } else {
        // Ningún sensor detectado → Modo LOW POWER
        currentMode = MODE_LOWPOWER;
        LED_ML_Off(); // asegúrate de apagar el LED ML
        PRINT_Mensaje("==> Ningún sensor conectado, sistema en modo LOW-POWER");
    }
}
