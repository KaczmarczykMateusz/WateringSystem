/*
 ============================================================================
 Name        : adc.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description : You can increase range with voltage divider
			Calibration with ref_adc and ref_v
 ============================================================================
 */
#ifndef ADC_H
#define ADC_H

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include "keyboard.h"

#define ADCIN PA7   //definition ADCIN (ADC input)

typedef struct {
	uint16_t adc_mid;
	uint16_t ref_adc; // set value in the middle of measure range which can be high because voltage divider
	uint16_t ref_v; // set value (volt) of adc which set during measuring of ref_adc (multiply this value *100)
	uint16_t adcVoltRaw;
	char beforeComa[3];
	char afterComa[3];
} TVOLT;

void adcInit(void);
void voltAdc(uint16_t adc, TVOLT * voltage);
uint16_t adcConvert(uint8_t channel);
uint16_t adcOversample(uint8_t channel, uint16_t extend);

#endif
