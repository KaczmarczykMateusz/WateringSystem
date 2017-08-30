/*	clock.h
 *
 *      Author: Mateusz Kaczmarczyk
 *      Microcontroller : Atmel AVR Atmega8
 *      16MHz external crystal resonator
 *      resolution 1 second
 *		uses HW timer1 atmega8
 */

#ifndef CLOCK_H
#define CLOCK_H
#include <avr/interrupt.h>

void clockInit(void);
void timeDivision(void);
void localTimeDivision(uint8_t, uint8_t, uint8_t);
uint32_t timeToSeconds(uint8_t, uint8_t, uint8_t);
void setTimeF(volatile uint8_t *, uint8_t);

volatile uint8_t timeChanged;
volatile uint8_t second, minute, hour, day;
uint8_t checkPreset;
uint32_t userTurnOnTime;




#endif
