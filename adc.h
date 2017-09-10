/*
 ============================================================================
 Name        : adc.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#ifndef ADC_H
#define ADC_H

#include <stdio.h>
#include <avr/io.h>
#include "keyboard.h"

#define ADCIN PA7   //definition ADCIN (ADC input)
#define ADC_CALIBRATE -0.49

#define maxAdc13Bit 8192 // 15 bit adc = 2^13
#define maxAdc15Bit 32768 // 15 bit adc = 2^15
#define ADC_15_BIT_DIVISION 0.000078125
#define ADC_13_BIT_DIVISION 0.0003125

uint16_t maxAdc;

void adcInitAvcc(void);
uint16_t adcConvert(void);
uint16_t adcOversampleAccurate(void);
uint16_t adcOversampleEfficient(void);
float efficientAdcVolt(void);
float accurateAdcVolt(void);

#endif
