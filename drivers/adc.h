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

#include "../include/common.h"

//@Brief: General all analog input PORT definitions (all ADC inputs belong to the same port in Atmega32 and Atmega8)
#define ANALOG_INPUT_PORT	PORTA
#define ANALOG_INPUT_DIR	DDRA
#define ANALOG_INPUT_PIN	PINA

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

TVOLT initAdcStruct(uint16_t refV, uint16_t refAdcVal);

#endif
