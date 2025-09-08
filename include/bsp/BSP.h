#ifndef BSP_H
#define BSP_H

#include <Arduino.h>

// === Selección de placa ===
#define ARDUINO_UNO
//#define ESP32 

// ==== Pines Arduino UNO ====
#if defined(ARDUINO_UNO)
#define TEMP_PIN A0
#define LUZ_PIN A1
#define HUM_PIN A2
#define BUTTON_PIN 2
#define LED_PIN 13

#define TEMP_DET 4
#define HUM_DET 5
#define LUZ_DET 6

#define VREF 5.0f
#define VREF_LUX 2.0f
#define ADCMAX 1023.0f

// ==== Pines ESP32 ====
#elif defined(ESP32)
#define TEMP_PIN 34
#define LUZ_PIN 35
#define HUM_PIN 32
#define BUTTON_PIN 4

#define LED_PIN 2 // LED integrado de la ESP32

#define TEMP_DET 26
#define HUM_DET 27
#define LUZ_DET 14

#define VREF 5.0f
#define VREF_LUX 4.0f
#define ADCMAX 4095.0f
#endif

// ================== Variables globales ==================
extern float temperaturaLM35;
extern float humedad;
extern float luz;
extern bool systemOn;

// ================== ADC ==================
void ADC_Init(int pin);
float ADC_Read(int pin);

// ================== GPIO ==================
void GPIO_Init(uint8_t pin, uint8_t mode);
void GPIO_Write(uint8_t pin, uint8_t value);
int  GPIO_Read(uint8_t pin);
void GPIO_PullUp(uint8_t pin);

// ================== LED ==================
void LED_Init();
void LED_On();
void LED_Off();

// ================== Print / Serial ==================
void PRINT_Temp(float temp);
void PRINT_Humedad(float hum);
void PRINT_Luz(float luz);

// --- NUEVOS helpers de impresión ---
void PRINT_SensorNoAvailable(const char* nombre); // imprime: "<nombre>: No_Disponible"
void PRINT_Mensaje(const char* msg);               // envoltura general de Serial.println

// ================== BOTÓN ==================
#define MODE_OFF       0
#define MODE_LOWPOWER  1
#define MODE_RUN       2
extern int currentMode;

void Button_Init();
void Button_Update();

// ================== Sensores ==================
void Sensors_Init();
bool SensorConnected(int pinDet);
void UpdateModeBySensors();

#endif
