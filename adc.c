/*
 ============================================================================
 Name        : adc.c
 Author      : Mateusz Kaczmarczyk
 Version     : Microcontroller : Atmel AVR Atmega32
 Description : See header file
 ============================================================================
 */
#include "adc.h"

/*************************************************************************
 Function: 	adcInit()
 Purpose:	Initialise 10-bit ADC converter
 **************************************************************************/
void adcInit(void) {

	ADCSRA |= (1<<ADEN) //ADC Enable

	| (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2); // set prescaler 128

//	ADMUX  =  (1<<REFS1) | (1<<REFS0) //Internal 2.56V Voltage Reference with external capacitor at AREF pin
	ADMUX  =  (1<<REFS0); //Internal 5V Voltage Reference with external capacitor at AREF pin

	DDRA &= ~(0x80); //Set PIN ADC as input (for some case) otherwise it may be omitted

}

/*************************************************************************
 Function: 	_adcConvert()
 Purpose:	Single ADC measurement
 Input:		uint8_t: bit value from datasheet
 Returns:	uint16_t: 10-bit ADC result
 **************************************************************************/
uint16_t _adcConvert(uint8_t channel)
{
	ADMUX &= ~0x0F;
	ADMUX |= channel;

	ADCSRA |= (1<<ADSC); // single conversion

	//wait until conversion finish
	while(ADCSRA & (1<<ADSC))
	{}

	return ADC;
}

/*************************************************************************
 Function: 	adcOversample()
 Purpose:	Complex ADC measurement including oversampling
 Input:		uint8_t: bit value from datasheet, uint8_t: how many bits to extend ADC
 Returns:	uint16_t: oversampled result value in n-bit resolution
 **************************************************************************/
uint16_t adcOversample(uint8_t channel, uint8_t extend) {
	uint16_t nyquistFreq = 4;
	for(int i=0; i<extend; i++) {
		nyquistFreq *= 2;
	}

	uint32_t adcSum = 0;
	for(int i=0; i<nyquistFreq; i++) {
		adcSum = adcSum + _adcConvert(channel);
	}
	adcSum = adcSum >> extend; // decimation

	return (uint16_t)adcSum;
}

/*************************************************************************
 Function: 	voltAdc()
 Purpose:
 Input:		uint16_t oversampled ADC, calibrated TVOLT struct
 Returns:	None, but sets TVOLT values
 **************************************************************************/
void voltAdc(uint16_t adc, TVOLT * voltage) {

	uint16_t adcVoltage = (adc * (uint32_t)voltage->ref_v) / voltage->ref_adc;
	voltage->adcVoltRaw = adcVoltage;
	div_t divMod = div(adcVoltage, 100);
	itoa(divMod.quot, voltage->beforeComa, 10);
	itoa(divMod.rem, voltage->afterComa, 10);
	// Make sure that digits after coma are set properly
	if(divMod.rem < 10) {
		voltage->afterComa[0] = '0';
		voltage->afterComa[1] = divMod.rem+'0'; // convert to ASCII
	}
	voltage->afterComa[2] = '0';
	voltage->afterComa[2] = '0';
}
