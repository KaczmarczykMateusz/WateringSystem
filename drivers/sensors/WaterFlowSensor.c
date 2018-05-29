/*
 ============================================================================
 Name        : waterFlowSensor.c
 Author      : Mateusz Kaczmarczyk
 Version     : Atmel AVR Atmega8
			   External quartz 16MHz
 Description : Waterflow sensor YF-S201 connected to ICP1 PIN
 ============================================================================
 */
#include <avr/io.h>
#include <util/delay.h>
#include "WaterFlowSensor.h"	//TODO change name of file to start from lowercase character

/*************************************************************************
 Function:	Interrupt from Timer1()
 Purpose:	Measure water flow and indicate while it's running
 **************************************************************************/
ISR(TIMER1_CAPT_vect) {
	if(TCNT1 >= 32768) {
		TCCR1B &= ~(1<<CS12) & ~(1<<CS11) & ~(1<<CS12);	// Stop timer1
		timerVal = ICR1;
		wfStatus &= ~WF_COUNT_RUNNING;
	} else {
		wfCount++;
		wfStatus |= WF_COUNT_RUNNING;
	}
	if(!(wfStatus & WF_COUNT_STARTED)) {	//Confirm that some waterflow was captured
		wfStatus |= WF_COUNT_STARTED;
	}
}

/*************************************************************************
 Function:	wfSensToggle()
 Purpose:	Start and stop water flow sensor reading
 Input:		Enables water flow measurement
 **************************************************************************/
void wfSensToggle(uint8_t enable) {
	if(!enable){
		wfStatus |= WF_COUNT_SUSPEND;
		return;
	} else if(wfStatus & WF_COUNT_SUSPEND) {
		wfStatus &= ~WF_COUNT_SUSPEND;
		wfStatus &= ~WF_COUNT_RUNNING;
	}

	if(	(TCNT1 > 60000) &&					//Prevent from stopping in dead point while pump stopped
		(wfStatus & WF_COUNT_STARTED)) {	//only in case if wf sensor captured some flow already otherwise do nothing
		wfStatus &= ~WF_COUNT_STARTED;
		TCCR1B &= ~(1<<CS12) & ~(1<<CS11) & ~(1<<CS12);	// Stop timer1 clearing prescalers
		wfStatus &= ~WF_COUNT_RUNNING;						// Let timer1 to restart
	}

	if(wfStatus & WF_COUNT_RUNNING) {	// Prevent from Starting timer while it's already running
		return;
	}
	TIMSK |= (1<<TICIE1);			// Enable Input Capture Interrupts
	TCNT1 = 0;						// Clear timer1 counter
	ICR1 = 0;						// Clear timer1 Input Capture Register
	TCCR1B = (1<<ICES1) | (1<<CS12);// choose source of interrupt (ICP1) rising edge and set prescaler 256, Start timer

}


/*************************************************************************
 Function:	measureWF()
 Purpose:	Measure water flow using data captured from timer and ISR
 Returns:	Water flow as [ml/minute]
 **************************************************************************/
uint16_t measureWF(void) {
	if(!timerVal) {
		return 0;
	}

	uint32_t wf = wfCount;
	wfCount = 0;

	//((Max counter val / real counter val) * impulses generated during measurement) * (max counter val / counter value equal to 1 second)
	wf = ((65536.0f / timerVal) * wf) * (65536 / 62500.0f); //Result as Hz

	//Pattern: (Frequency x 60) / 7.5Q(constant), = (flow rate [l]/h litres per hour)
	//(Frequency x 60000) / 75Q(constant), = (flow rate  per hour)
	wf = (wf * 60000) / 75;

	timerVal = 0;


	return (uint16_t)round(wf/6); // Return [ml/minute]
}
