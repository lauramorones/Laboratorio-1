#ifndef BSP_H
#define BSP_H

#include <Arduino.h>
#include <math.h>   // Usamos math.h (estándar C)
#include <stdlib.h> // Usamos stdlib.h (estándar C)

// === Selección de placa (Comenta/descomenta la que NO uses) ===
#define ARDUINO_UNO
//#define ESP32 

// ================== Constantes del Perceptrón ==================
#define N_DIMENSIONS 5
#define N_ROWS 32 // 2^N_DIMENSIONS

// ==== Pines Arduino UNO ====
#if defined(ARDUINO_UNO)
// Pines para los 5 potenciometros (entradas analógicas)
#define POT1_PIN A0
#define POT2_PIN A1
#define POT3_PIN A2
#define POT4_PIN A3
#define POT5_PIN A4

// Pines auxiliares de detección para cada potenciómetro
// Se recomienda conectar cada pin DET a GND cuando el pot esté presente
#define POT1_DET_PIN 3
#define POT2_DET_PIN 4
#define POT3_DET_PIN 5
#define POT4_DET_PIN 6
#define POT5_DET_PIN 7

#define BUTTON_PIN 2
#define LED_PIN 13

#define VREF 5.0f
#define ADCMAX 1023.0f
#define ADC_THRESHOLD (ADCMAX / 2.0f)

// ==== Pines ESP32 ====
#elif defined(ESP32)
// Pines para los 5 potenciometros (Entradas X1 a X5)
#define POT1_PIN 34
#define POT2_PIN 35
#define POT3_PIN 32
#define POT4_PIN 33
#define POT5_PIN 25

// Pines auxiliares de detección para cada potenciómetro
// Conectar a GND cuando el pot esté presente
#define POT1_DET_PIN 18
#define POT2_DET_PIN 19
#define POT3_DET_PIN 21
#define POT4_DET_PIN 22
#define POT5_DET_PIN 23

#define BUTTON_PIN 4
#define LED_PIN 2 // LED integrado de la ESP32

#define VREF 3.3f // VRef común en ESP32
#define ADCMAX 4095.0f
#define ADC_THRESHOLD (ADCMAX / 2.0f)
#endif

// ================== Variables globales ==================
extern bool systemOn;
extern int currentMode;
// Número de entradas (potenciómetros) detectados dinámicamente
extern int N_inputs;

// ================== ADC ==================
void ADC_Init(int pin);

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
void PRINT_Mensaje(const char* msg);
void PRINT_Perceptron_Status(int* inputs, int output);
void PRINT_Perceptron_Weights(float* weights);

// ================== BOTÓN ==================
#define MODE_OFF      0
#define MODE_RUN      2 // Solo tendremos OFF y RUN

void Button_Init();
void Button_Update();

// ================== Perceptrón ==================
void PERCEPTRON_Init_Training(); // Inicia y entrena el perceptrón vía Serial
void PERCEPTRON_Run_Update();    // Lee pots y ejecuta el modelo
int  PERCEPTRON_Get_Output();    // Devuelve la última salida (0 o 1)

#endif
