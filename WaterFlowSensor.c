// Waterflow sensor YF-S201
// Microcontroller : Atmel AVR Atmega8
// External quartz 16MHz
// Author: Mateusz Kaczmarczyk
// Date:19.08.2017

#include <avr/io.h>
#include <util/delay.h>
#include "WaterFlowSensor.h"


// measure distance function
double WaterFlow (void) {
	waterflowAux = 0;

	MCUCR |= (1<<ISC01) | (1<<ISC00);	// choose source of interrupt (INT0) rising edge
	
	GICR |= (1<<INT0); 	// enable external interrupts at INT0
	_delay_ms(1000);
	waterflow = (double) waterflowAux;

	waterflow = waterflow*60/7.5;  //(Pulse frequency x 60) / 7.5Q, = (flow rate)
	waterflow = waterflow/60; // convert to L/second
	
	GICR &= ~(1<<INT0);	// disable external interrupts at INT0
	return waterflow;
}

double trigger(void) {
	uint8_t wf = 0;
	waterflowAux=0;
	MCUCR |= (1<<ISC01) | (1<<ISC00);	// choose source of interrupt (INT0) rising edge
	GICR |= (1<<INT0); 	// enable external interrupts at INT0
	_delay_ms(200);

	if(waterflowAux > 1)
		wf = WaterFlow();
			
	GICR &= ~(1<<INT0);	// disable external interrupts at INT0
			
	return wf;
}
	
ISR(INT0_vect) {
	waterflowAux++;
}
