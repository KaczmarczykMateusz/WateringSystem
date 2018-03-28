/*
 ============================================================================
 Name        : clock.c
 Author      : Mateusz Kaczmarczyk
 Version     : Atmega32 with 32 768 kHz cristal at TOSC1 & TOSC2 (Timer2)
 Description : See header file
 ============================================================================
 */
#include <avr/sleep.h>
#include <avr/io.h>
#include <util/delay.h>
#include "clock.h"
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(TIMER2_OVF_vect)
/*************************************************************************
 Function: Interrupt from Timer2
 Purpose:  Counting passed seconds and setting time change flag
 **************************************************************************/
{
	++second;
	SEC_CHANGED_SET;
}

/*************************************************************************
 Function: rt_clock_init()
 Purpose:  initialize RTC Timer2 32 768 kHz clock
 Input:    none
 Returns:  none
		note that order	in function is important !
 **************************************************************************/
void rt_clock_init(void) {
	ASSR = _BV(AS2);						// Set timer2 to run asynchronous
	TCCR2 = _BV(CS20) | _BV(CS22);			// Start timer2 with prescaler = 128 (TCNT2 = 255 takes 1 sec.)
	while(ASSR & _BV(TCN2UB));				// Wait until TC2 is updated
	TIMSK = _BV(TOIE2);						// Enable timer2 overflow interrupt
	sei();
}

/*************************************************************************
 Function: timeDivision()
 Purpose:  Puts counted by clock seconds into DD:HH:MM:SS format
 Input:    struct with time to be ordered
 Returns:  none
 **************************************************************************/
void timeDivision(time *tmp) {
	if (tmp->second>59) {
		tmp->minute++;
		tmp->second = 0;
		MIN_CHANGED_SET;
	}
	if (tmp->minute>59) {
		tmp->hour++;
		tmp->minute=0;
		if(tmp->hour>23) {
			tmp->hour = 0;
			tmp->day++;
			DAY_CHANGED_SET;
		}
	}
	if(second>59) {
		second = 0;
	}
}

/*************************************************************************
 Function: timeToSeconds()
 Purpose:  Transform time HH:MM into seconds
 Input:    Struct with time to be transformed
 Notice Resolution: minutes
 **************************************************************************/
uint32_t timeToSeconds(time *tmp){
	return (uint32_t)((uint32_t)((tmp->hour * 60) + tmp->minute) * 60);
}

/*************************************************************************
 Function: timeToSecondsAccurate()
 Purpose:  Transform time HH:MM:SS into seconds
 Input:    Struct with time to be transformed
 Notice Resolution: seconds
 **************************************************************************/
uint32_t timeToSecondsAccurate(time *tmp){
	return (uint32_t)((uint32_t)(((tmp->hour * 60) + tmp->minute) * 60) + tmp->second);
}

/*************************************************************************
 Function: getCurrentTime()
 Purpose:  Transform time from passed struct into seconds
 Input:    Time hours, minutes and seconds into seconds
 Returns:  Time as seconds
 **************************************************************************/
uint32_t getCurrentTime(time *tmp) {
	return timeToSeconds(tmp);
}

/*************************************************************************
 Function: powerSaveInit()
 Purpose:  Initialise power saving/ sleep mode without entering it
 	 	   it enables using function goToSleep()
 Input:    none
 Returns:  none
 **************************************************************************/
void powerSaveInit(void){
	rt_clock_init();  //initialise the timer (opitonally if not initialised before)
	sei();
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
}

/*************************************************************************
 Function: goToSleep()
 Purpose:  Enters power saving/ sleep mode, it has to be preceded by powerSaveInit()
 Input:    none
 Returns:  none
 **************************************************************************/
void goToSleep(void) {
	sleep_enable();
	sleep_mode();
	sleep_disable();
}

/*************************************************************************
 Function: timeToMinutes()
 Purpose:  Transform time HH:MM into minutes
 Input:    Struct with time to be transformed
 **************************************************************************/
uint32_t timeToMinutes(time *tmp) {
	return (uint32_t)((uint32_t)(tmp->hour * 60) + tmp->minute);
}
