/*
 ============================================================================
 Name        : adc.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "adc.h"
// implement initialisation in main

void adcInit(void) {

	ADCSRA |= (1<<ADEN) //ADC Enable

	//ADPS2:0: set prescaler 128
	|(1<<ADPS0)
	|(1<<ADPS1)
	|(1<<ADPS2);

//	ADMUX  =  (1<<REFS1) | (1<<REFS0) //Internal 2.56V Voltage Reference with external capacitor at AREF pin
	ADMUX  =  (1<<REFS0) //Internal 5V Voltage Reference with external capacitor at AREF pin
	| (1<<MUX0) | (1<<MUX1) | (1<<MUX2); //Input Channel Selections (ADC7 - Pin 7 )

	DDRA &= ~(0x80); //Set PIN ADC as input (for some case) otherwise it may be omitted

}

uint16_t adcConvert(uint8_t channel)
{
//	ADMUX |= channel;

	ADCSRA |= (1<<ADSC); // single conversion

	//wait until conversion finish
	while(ADCSRA & (1<<ADSC))
	{}

	return ADC;
}

uint16_t adcOversample(uint8_t channel, uint16_t extend) { //extend - how many bits resolution to add to oversampling
	uint16_t nyquistFreq = 4;
	for(int i=0; i<extend; i++) {
		nyquistFreq *= 2;
	}

	uint32_t adcSum = 0;
	for(int i=0; i<nyquistFreq; i++) {
		adcSum = adcSum + adcConvert(channel);
	}
	adcSum = adcSum >> extend; // decimation

	return (uint16_t)adcSum;
}

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
