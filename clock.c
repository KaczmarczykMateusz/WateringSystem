/*
 ============================================================================
 Name        : clock.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include <avr/sleep.h>
#include <avr/io.h>
#include <util/delay.h>
#include "clock.h"
#include <avr/interrupt.h>
#include <util/delay.h>

// RTC time is thru timer2 being run by a 32.768kHz xtal on pins TOSC1&2
// note that order is important
void rt_clock_init(void)
{
	_delay_ms(2000);
	ASSR = _BV(AS2);						// Set timer2 to run asynchronous
	TCCR2 = _BV(CS20) | _BV(CS22);			// Start timer2 with prescaler = 128 (TCNT2 = 255 takes 1 sec.)
	while(ASSR & _BV(TCN2UB));				// Wait until TC2 is updated
	TIMSK = _BV(TOIE2);						// Enable timer2 overflow interrupt
	sei();
}

// Setting time with push button
void setTime(tButton * btn, time *tmp) {
	static uint8_t longPress;
	static uint8_t shortenDelay;
	register uint8_t key_press = (*btn->KPIN & btn->key_mask);
	if (!(key_press)) {
		tmp->minute++;
		second = 0;
		if (longPress > 30) {
			tmp->hour ++;
		} else if (longPress > 15) {
			tmp->minute += 30;
		} else if (longPress > 0) {
			tmp->minute += 3;
		}

		timeDivision(tmp);
		if(shortenDelay == 0) {
			_delay_ms(120);
		}
		_delay_ms(120);
		if (!(key_press)) {
			shortenDelay = 1;
			if (longPress <= 26) {
				longPress++;
			}
		}
	timeChanged = 1;
	} else {
		longPress = 0;
		shortenDelay = 0;
	}
}

void timeDivision(time *tmp) {
	if (second>59) {
		tmp->minute++;
		tmp->second = 0;
		second = 0;
	}
	if (tmp->minute>59) {
		tmp->hour++;
		tmp->minute=0;
	}
	if(tmp->hour>23) {
		tmp->hour = 0;
		tmp->day++;
	}
}

uint32_t timeToSeconds(time *tmp){
	return ((tmp->hour * 60) + tmp->minute)*60; //TODO: casting

}

void userTimer(int *turnOnTime, int *turnOffTime, void (*action1)(void), void (*action2)(void), time *tmp ) {
	if(tmp->minute == *turnOnTime) {
		if(checkPreset == 0) {
			checkPreset= 1;
			if(action1) action1();
		}
	}

	if(tmp->minute == *turnOffTime) {
		if(checkPreset == 1) {
			checkPreset = 0;
			if(action2) action2();
		}
	}
}

uint32_t getCurrentTime(time *tmp) {
	timeToSeconds(tmp);
	return saveTime;
}

void powerSaveInit(void){
	rt_clock_init();  //initialise the timer (opitonally if not initialised before)
	sei();
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
}

void goToSleep(void) {
	sleep_enable();
	sleep_mode();
	sleep_disable();
}


ISR(TIMER2_OVF_vect)
{
	++second;
	timeChanged = 1;
}
