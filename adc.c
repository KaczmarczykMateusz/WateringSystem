/*
 ============================================================================
 Name        : adc.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "adc.h"

void adcInit(void) {
	ADCSRA =   (1<<ADEN) //ADC Enable

	//ADPS2:0: set prescaler 128
	|(1<<ADPS0)
	|(1<<ADPS1)
	|(1<<ADPS2);

	ADMUX  =  (1<<REFS1) | (1<<REFS0) //REFS1:0: Reference Selection Bits
		 //Internal 2.56V Voltage Reference with external capacitor at AREF pin
	| (1<<MUX1) ; //Input Channel Selections (ADC7 - Pin 7 )

	DDRC &=~ (1<<ADCIN); //Set PIN ADC as input (for some case) otherwise it may be omitted
}

uint16_t adcConvert(void)
{
	ADCSRA |= (1<<ADSC); // single conversion

	//wait until conversion finish
	while(ADCSRA & (1<<ADSC))
	{}

	return ADC;
}

uint16_t adcOversampling(void) {
	uint32_t adcSum = 0;
	for(int i=0; i<1024; i++) {
		adcSum = adcSum + adcConvert();
	}
	adcSum = adcSum >> 5; // decimation

	return (uint16_t)adcSum;
}
