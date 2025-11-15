#ifndef BSP_H
#define BSP_H

#include <Arduino.h>
#include <math.h>   
#include <stdlib.h> 

// === Selección de placa ===
//#define ARDUINO_UNO
#define ESP32 

// ================== Perceptrón ==================
#define N_DIMENSIONS 5
#define N_ROWS 32   // 2^5

// ==== Pines Arduino UNO ====
#if defined(ARDUINO_UNO)
#define POT1_PIN A0
#define POT2_PIN A1
#define POT3_PIN A2
#define POT4_PIN A3
#define POT5_PIN A4

#define BUTTON_PIN 2
#define LED_PIN 13

#define VREF 5.0f
#define ADCMAX 1023.0f
#define ADC_THRESHOLD (ADCMAX / 2.0f)

// ==== Pines ESP32 ====
#elif defined(ESP32)
#define POT1_PIN 34
#define POT2_PIN 35
#define POT3_PIN 32
#define POT4_PIN 33
#define POT5_PIN 25

#define BUTTON_PIN 4
#define LED_PIN 2

#define VREF 3.3f
#define ADCMAX 4095.0f
#define ADC_THRESHOLD (ADCMAX / 2.0f)
#endif

// ================== Variables globales ==================
extern bool systemOn;
extern int currentMode;
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

// ================== Print ==================
void PRINT_Mensaje(const char* msg);
void PRINT_Perceptron_Status(int* inputs, int output);
void PRINT_Perceptron_Weights(float* weights);

// ================== BOTÓN ==================
#define MODE_OFF 0
#define MODE_RUN 2

void Button_Init();
void Button_Update();

// ================== Perceptrón ==================
void PERCEPTRON_Init_Training();
void PERCEPTRON_Run_Update();
int  PERCEPTRON_Get_Output();

#endif
