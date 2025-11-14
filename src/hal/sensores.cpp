#include "bsp/BSP.h"

int potPins[5] = { POT1_PIN, POT2_PIN, POT3_PIN, POT4_PIN, POT5_PIN };

bool Pot_Connected(int index) {
    float v = ADC_Read(potPins[index]);
    return (v > 0.05f);
}

// Cuenta cuántos potenciómetros están conectados
int Detect_Pots() {
    int count = 0;

    for (int i = 0; i < 5; i++) {
        if (Pot_Connected(i))
            count++;
    }

    // Asigna modo automáticamente
    if (!systemOn) {
        currentMode = MODE_OFF;
    }
    else if (count == 0) {
        currentMode = MODE_LOWPOWER;
    }
    else {
        currentMode = MODE_RUN;
    }

    return count;
}
