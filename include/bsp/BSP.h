#ifndef BSP_H
#define BSP_H

#include <Arduino.h>

// =============================================================
//            Selección de tarjeta (DEFINIR SOLO UNA)
// =============================================================
#define ARDUINO_UNO
//#define ESP32

// =============================================================
//                  CONFIGURACIÓN ARDUINO UNO
// =============================================================
#if defined(ARDUINO_UNO)

// ----- Pines de POTENCIÓMETROS -----
#define POT1_PIN A0
#define POT2_PIN A1
#define POT3_PIN A2
#define POT4_PIN A3
#define POT5_PIN A4

// ----- Pines de LED -----
#define LED_PIN 13
#define LED_ML_PIN 12       // LED especial para ML / perceptrón

// ----- Botón -----
#define BUTTON_PIN 2        // Botón de encendido/apagado

// ----- Voltajes del ADC -----
#define VREF 5.0f
#define ADCMAX 1023.0f

// =============================================================
//                        CONFIGURACIÓN ESP32
// =============================================================
#elif defined(ESP32)

// ----- Pines de POTENCIÓMETROS (ADC1 SEGUROS) -----
#define POT1_PIN 34   // ADC1_CH6 (INPUT ONLY)
#define POT2_PIN 35   // ADC1_CH7 (INPUT ONLY)
#define POT3_PIN 32   // ADC1_CH4
#define POT4_PIN 33   // ADC1_CH5
#define POT5_PIN 39   // ADC1_CH3 (INPUT ONLY)

// ----- Pines de LED -----
#define LED_PIN 2         // LED general en la mayoría de ESP32
#define LED_ML_PIN 21     // LED especial para ML / perceptrón

// ----- Botón -----
#define BUTTON_PIN 4      // Pin sugerido para botón

// ----- Voltajes del ADC -----
#define VREF 3.3f
#define ADCMAX 4095.0f

#endif

// =============================================================
//                         MODOS DEL SISTEMA
// =============================================================
#define MODE_OFF       0
#define MODE_LOWPOWER  1
#define MODE_RUN       2

extern int currentMode;   // modo del sistema
extern bool systemOn;     // estado ON/OFF

// =============================================================
//                          PROTOTIPOS ADC
// =============================================================
void ADC_Init(int pin);
float ADC_Read(int pin);

// =============================================================
//                          PROTOTIPOS GPIO
// =============================================================
void GPIO_Init(uint8_t pin, uint8_t mode);
void GPIO_Write(uint8_t pin, uint8_t value);
int  GPIO_Read(uint8_t pin);
void GPIO_PullUp(uint8_t pin);

// =============================================================
//                          PROTOTIPOS LED
// =============================================================
void LED_Init();
void LED_On();
void LED_Off();
void LED_ML_On();
void LED_ML_Off();

// =============================================================
//                   BOTÓN ENCENDIDO/APAGADO
// =============================================================
void Button_Init();
void Button_Update();

// =============================================================
//                 IMPRESIONES (SERIAL PRINT HELPERS)
// =============================================================
void PRINT_Mensaje(const char* msg);

#endif
