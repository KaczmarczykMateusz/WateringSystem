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

#define ADCIN PA2   //definition ADCIN (ADC input)
#define ADC_CALIBRATE 0.43

#define maxAdc13Bit 8192 // 15 bit adc = 2^13
#define maxAdc14Bit 16384 // 15 bit adc = 2^14
#define maxAdc15Bit 32768 // 15 bit adc = 2^15

uint16_t maxAdc;

void adcInit(void);
uint16_t adcConvert(void);
uint16_t adcOversampling(void);

#endif
