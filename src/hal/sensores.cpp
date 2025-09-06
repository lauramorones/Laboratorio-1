#include "bsp/BSP.h"

void Sensors_Init() {
    GPIO_PullUp(TEMP_DET);
    GPIO_PullUp(HUM_DET);
    GPIO_PullUp(LUZ_DET);
}

bool SensorConnected(int pinDet) {    // El pin está en HIGH cuando no hay sensor, LOW cuando sí hay sensor
    if (digitalRead(pinDet) == LOW) {
        return true;  // Sensor conectado
    } else {
        return false; // Sensor desconectado
    }
}

void UpdateModeBySensors() {
    if (systemOn == 0) {
        currentMode = MODE_OFF; // OFF
        return;
    }

    if (SensorConnected(TEMP_DET) || SensorConnected(HUM_DET) || SensorConnected(LUZ_DET)) {

        // Temperatura
        if (SensorConnected(TEMP_DET)) {
            PRINT_Temp(ADC_Read(TEMP_PIN));
        } else {
            PRINT_SensorNoAvailable("Temperatura");
        }

        // Humedad
        if (SensorConnected(HUM_DET)) {
            PRINT_Humedad(ADC_Read(HUM_PIN));
        } else {
            PRINT_SensorNoAvailable("Humedad");
        }

        // Luz
        if (SensorConnected(LUZ_DET)) {
            PRINT_Luz(ADC_Read(LUZ_PIN));
        } else {
            PRINT_SensorNoAvailable("Luz");
        }

        currentMode = MODE_RUN; // RUN
        PRINT_Mensaje("Sistema en modo RUN");

    } else {
        currentMode = MODE_LOWPOWER; // LOWPOWER
        PRINT_Mensaje("==> Ningun sensor conectado, sistema en modo LOW-POWER");
    }
}
