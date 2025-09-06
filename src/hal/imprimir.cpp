#include "bsp/BSP.h"

void PRINT_Temp(float temp) {
  Serial.print("Temperatura: ");
  Serial.println(temp);
}

void PRINT_Humedad(float hum) {
  Serial.print("Humedad: ");
  Serial.println(hum);
}

void PRINT_Luz(float luz) {
  Serial.print("Luz: ");
  Serial.println(luz);
}

// === FUNCIONES NO Disponible ===
void PRINT_SensorNoAvailable(const char* nombre) {
  Serial.print(nombre);
  Serial.println(": No_Disponible");
}

void PRINT_Mensaje(const char* msg) {
  Serial.println(msg);
}
