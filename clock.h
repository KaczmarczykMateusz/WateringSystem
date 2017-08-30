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

void clockInit(void);
void timeDivision(void);
void localTimeDivision(uint8_t day, uint8_t hour, uint8_t minute);
uint16_t timeToSeconds(uint8_t, uint8_t, uint8_t);

volatile uint8_t timeChanged;
volatile uint8_t second, minute, hour, day;
uint8_t checkPreset;
uint32_t userTurnOnTime;
uint32_t userTurnOnTime;


typedef struct _setTime{
uint8_t second; 
uint8_t minute; 
uint8_t hour; 
uint8_t day;
}setTime;

#endif
