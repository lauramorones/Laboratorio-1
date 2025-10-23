#include "bsp/BSP.h"
#include "ml/ml_layer.h"

void Sensors_Init() {
    GPIO_PullUp(TEMP_DET);
    GPIO_PullUp(HUM_DET);
    GPIO_PullUp(LUZ_DET);
}

bool SensorConnected(int pinDet) {
    return (digitalRead(pinDet) == LOW);
}

void UpdateModeBySensors() {
    // 🔹 Si el sistema está apagado, apaga ambos LEDs y sal de la función
    if (!systemOn) {
        currentMode = MODE_OFF;
        LED_Off();
        LED_ML_Off();  // 🔥 Corrección: se apaga también el LED del ML
        PRINT_Mensaje("Sistema apagado -> LEDs OFF");
        return;
    }

    // 🔹 Si hay sensores conectados
    if (SensorConnected(TEMP_DET) || SensorConnected(HUM_DET) || SensorConnected(LUZ_DET)) {

        float temp = 0, hum = 0, luzVal = 0;

        // === Lectura y visualización ===
        if (SensorConnected(TEMP_DET)) {
            temp = ADC_Read(TEMP_PIN);
            PRINT_Temp(temp);
        } else {
            PRINT_SensorNoAvailable("Temperatura");
        }

        if (SensorConnected(HUM_DET)) {
            hum = ADC_Read(HUM_PIN);
            PRINT_Humedad(hum);
        } else {
            PRINT_SensorNoAvailable("Humedad");
        }

        if (SensorConnected(LUZ_DET)) {
            luzVal = ADC_Read(LUZ_PIN);
            PRINT_Luz(luzVal);
        } else {
            PRINT_SensorNoAvailable("Luz");
        }

        // === Voltajes reales (para Processing) ===
        float voltTemp = (analogRead(TEMP_PIN) * VREF) / ADCMAX;
        float voltHum  = (analogRead(HUM_PIN) * VREF) / ADCMAX;
        float voltLuz  = (analogRead(LUZ_PIN) * VREF) / ADCMAX;

        Serial.print("Pot 1: ");
        Serial.println(voltTemp, 3);
        Serial.print("Pot 2: ");
        Serial.println(voltHum, 3);
        Serial.print("Pot 3: ");
        Serial.println(voltLuz, 3);

        // === Machine Learning ===
        int decision = ML_Predict(voltTemp, voltHum, voltLuz);

        if (decision == 1) {
            LED_ML_On();
            PRINT_Mensaje("ML: LED ON (Clase 1)");
        } else {
            LED_ML_Off();
            PRINT_Mensaje("ML: LED OFF (Clase 0)");
        }

        currentMode = MODE_RUN;
        PRINT_Mensaje("Sistema en modo RUN");

    } else {
        // 🔹 Si no hay sensores conectados
        currentMode = MODE_LOWPOWER;
        LED_Off();
        LED_ML_Off();  // También se apaga el LED ML al no haber sensores
        PRINT_Mensaje("==> Ningún sensor conectado, sistema en modo LOW-POWER");
    }
}
