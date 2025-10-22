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
    if (!systemOn) {
        currentMode = MODE_OFF;
        LED_Off();
        return;
    }

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

        // === Voltajes reales (para comparar con dataset) ===
        float voltTemp = (analogRead(TEMP_PIN) * VREF) / ADCMAX;
        float voltHum  = (analogRead(HUM_PIN) * VREF) / ADCMAX;
        float voltLuz  = (analogRead(LUZ_PIN) * VREF) / ADCMAX;

        Serial.print("Voltajes -> TEMP: ");
        Serial.print(voltTemp, 3);
        Serial.print(" V | HUM: ");
        Serial.print(voltHum, 3);
        Serial.print(" V | LUZ: ");
        Serial.print(voltLuz, 3);
        Serial.println(" V");

        // === Machine Learning ===
        int decision = ML_Predict(temp, hum, luzVal);

        if (decision == 1) {
            LED_On();
            PRINT_Mensaje("ML: LED ON (Clase 1)");
        } else {
            LED_Off();
            PRINT_Mensaje("ML: LED OFF (Clase 0)");
        }

        currentMode = MODE_RUN;
        PRINT_Mensaje("Sistema en modo RUN");

    } else {
        currentMode = MODE_LOWPOWER;
        LED_Off();
        PRINT_Mensaje("==> Ningún sensor conectado, sistema en modo LOW-POWER");
    }
}
