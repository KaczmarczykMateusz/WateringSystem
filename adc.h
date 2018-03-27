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

//  @brief  holding values  for ADC oversampling measurement
typedef struct {
	// @brief   set value in the middle (not precise) of measure range which can be pre-stepped down with voltage divider
	uint16_t ref_adc;

	// @brief   set value (volt) of ADC which set during measuring of ref_adc (multiply this value *100)
	uint16_t ref_v;

	// @brief   voltage multiplied by *100
	uint16_t adcVoltRaw;

	// @brief  	digits to print as ASCII char before coma (xx. full volts)
	char beforeComa[3];

	// @brief   digits to print as ASCII char before coma (.xx of volts)
	char afterComa[3];
} TVOLT;

/**
 * @brief    Initialise ADC converter with 10 bit (maximum for Atmega32) resolution
 */
void adcInit(void);

/**
 * @brief	Counting voltage from n-bit oversampleed ADC value
 * @param	uint16_t result of adc oversample measurement in n-bit resolution,
 * @param	calibrated TVOLT srtuct holding values for ADC oversampling
 * @return  None, but modifies struct changing adcVoltRaw, beforeComa[] and afterComa[] values
 * @see     adcOversample()
 */
void voltAdc(uint16_t adc, TVOLT * voltage);

/**
 * @brief    Single ADC measurement without oversamppling nor averaging
 * @param    uint8_t: ADC channel as bit value set following Atmega datasheet
 * @return   uint16_t: measured value as 10-bit ADC result
 */
uint16_t _adcConvert(uint8_t channel);

/**
 * @brief	 Full ADC measurement including oversampling
 * @param    uint8_t: ADC channel as bit value set following Atmega datasheet
 * @param    uint8_t: how many bits of resolution to add to oversampling
 * @return   uint16_t: oversampled result value in n-bit resolution
 * 			 (n = extend + basic resolution)
 */
uint16_t adcOversample(uint8_t channel, uint8_t extend);

#endif
