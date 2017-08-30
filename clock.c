/*	clock.c
 *
 *      Author: Mateusz Kaczmarczyk
 */
#include <avr/io.h>
#include <util/delay.h>
#include "clock.h"

void clockInit(void) {
	TCCR1B |= (1 << WGM12); // CTC
	OCR1A = 15624;
	TCCR1B |= (1 << CS10) | (1 << CS12); //  prescaler 1024
	sei();
	TIMSK |= (1 << OCIE1A);
}
void timeDivision(void) {
	if (second>59) {
		minute++;
		second = 0;
	}
	if (minute>59) {
			hour++;
			minute=0;
	}
	if(hour>23) {
				hour = 0;
				day++;
	}
}

void setTimeF(volatile uint8_t * pin, uint8_t val) {
	if (!(*pin & val)) {
		second = 0;
		++minute;
		localTimeDivision(day, hour, minute);
		_delay_ms(150); // if key pressed longer it will increase speed
		if (!(*pin & val)) {
			minute += minute+3;
		}
	}


}


void localTimeDivision(uint8_t day, uint8_t hour, uint8_t minute) {
	second = 0;
	if (minute>59) {
			hour++;
			minute=0;
	}
	if(hour>23) {
				hour = 0;
				day++;
	}
}

uint32_t timeToSeconds(uint8_t hour, uint8_t minute, uint8_t second){
	return ((hour * 60) + minute)*60; //TODO: casting

}
typedef struct _setTime{
uint8_t second;
uint8_t minute;
uint8_t hour;
uint8_t day;
} setTime;

void userTimer(int *turnOnTime, int *turnOffTime, void (*action1)(void), void (*action2)(void) ) {
	if(minute == *turnOnTime) {
		if(checkPreset == 0) {
			checkPreset= 1;
			if(action1) action1();
		}
	}

	if(minute == *turnOffTime) {
		if(checkPreset == 1) {
			checkPreset = 0;
			if(action2) action2();
		}
	}
}

ISR(TIMER1_COMPA_vect) {
	timeChanged = 1;
	second++;
}
