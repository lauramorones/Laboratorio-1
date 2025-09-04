#include "bsp/BSP.h"

void Sensors_Init() {
    pinMode(TEMP_DET, INPUT_PULLUP);
    pinMode(HUM_DET, INPUT_PULLUP);
    pinMode(LUZ_DET, INPUT_PULLUP);
}

bool SensorConnected(int pinDet) {
    // El pin está en HIGH cuando no hay sensor, LOW cuando sí hay sensor
    if (digitalRead(pinDet) == LOW) {
        return true;  // Sensor conectado
    } else {
        return false; // Sensor desconectado
    }
}

void UpdateModeBySensors() {
    if (!systemOn) {
        currentMode = MODE_OFF; // OFF
        return;
    }

    if (SensorConnected(TEMP_DET) || SensorConnected(HUM_DET) || SensorConnected(LUZ_DET)) {

        // Temperatura
        if (SensorConnected(TEMP_DET)) {
            PRINT_Temperatura(ADC_Lectura(TEMP_PIN));
        } else {
            PRINT_SensorNoDisponible("Temperatura");
        }

        // Humedad
        if (SensorConnected(HUM_DET)) {
            PRINT_Humedad(ADC_Lectura(HUM_PIN));
        } else {
            PRINT_SensorNoDisponible("Humedad");
        }

        // Luz
        if (SensorConnected(LUZ_DET)) {
            PRINT_Luz(ADC_Lectura(LUZ_PIN));
        } else {
            PRINT_SensorNoDisponible("Luz");
        }

        currentMode = MODE_RUN; // RUN
        PRINT_Mensaje("Sistema en modo RUN");

    } else {
        currentMode = MODE_LOWPOWER; // LOWPOWER
        PRINT_Mensaje("==> Ningun sensor conectado, sistema en modo LOW-POWER");
    }
}
