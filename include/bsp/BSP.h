#ifndef BSP_H
#define BSP_H

#include <Arduino.h>
#include <math.h>
#include <stdlib.h>

// === Selección de placa ===
//#define ARDUINO_UNO
#define ESP32 

// ================== Constantes del Perceptrón ==================
#define N_DIMENSIONS 5
#define N_ROWS 32 // 2^5

// ================== Pines Arduino UNO ==================
#if defined(ARDUINO_UNO)
// Entradas analógicas
#define POT1_PIN A0
#define POT2_PIN A1
#define POT3_PIN A2
#define POT4_PIN A3
#define POT5_PIN A4

// Pines de detección
#define POT1_DET_PIN 3
#define POT2_DET_PIN 4
#define POT3_DET_PIN 5
#define POT4_DET_PIN 6
#define POT5_DET_PIN 7

// Botón y LEDS
#define BUTTON_PIN 2
#define LED_PERC_PIN 13          // LED del perceptrón (igual que antes)
#define LED_MODE_PIN 12          // 🆕 NUEVO LED indicador del sistema

#define VREF 5.0f
#define ADCMAX 1023.0f
#define ADC_THRESHOLD (ADCMAX / 2.0f)


// ================== Pines ESP32 ==================
#elif defined(ESP32)

#define POT1_PIN 34
#define POT2_PIN 35
#define POT3_PIN 32
#define POT4_PIN 33
#define POT5_PIN 25

#define POT1_DET_PIN 18
#define POT2_DET_PIN 19
#define POT3_DET_PIN 21
#define POT4_DET_PIN 22
#define POT5_DET_PIN 23

#define BUTTON_PIN 4
#define LED_PERC_PIN 15          // LED del perceptrón (existe)
#define LED_MODE_PIN 2        

#define VREF 3.3f
#define ADCMAX 4095.0f
#define ADC_THRESHOLD (ADCMAX / 2.0f)
#endif


// ================== Variables globales ==================
extern bool systemOn;
extern int currentMode;
extern int N_inputs;

// ================== MODO DEL SISTEMA ==================
#define MODE_OFF       0
#define MODE_RUN       2
#define MODE_LOWPOWER  3   // 

// ================== ADC ==================
void ADC_Init(int pin);

// ================== GPIO ==================
void GPIO_Init(uint8_t pin, uint8_t mode);
void GPIO_Write(uint8_t pin, uint8_t value);
int  GPIO_Read(uint8_t pin);
void GPIO_PullUp(uint8_t pin);

// ================== LEDS ==================
void LED_PERCEPTRON_Init();
void LED_PERCEPTRON_On();
void LED_PERCEPTRON_Off();

void LED_MODE_Init();     // 
void LED_MODE_On();       // 
void LED_MODE_Off();      // 

// ================== PRINT ==================
void PRINT_Mensaje(const char* msg);
void PRINT_Perceptron_Status(int* inputs, int output);
void PRINT_Perceptron_Weights(float* weights);

// ================== BOTÓN ==================
void Button_Init();
void Button_Update();

// ================== PERCEPTRÓN ==================
void PERCEPTRON_Init_Training();
void PERCEPTRON_Run_Update();
int  PERCEPTRON_Get_Output();

#endif
