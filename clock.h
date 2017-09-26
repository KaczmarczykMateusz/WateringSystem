/*
 ============================================================================
 Name        : clock.h
 Author      : Mateusz Kaczmarczyk
 Version     : Microcontroller : Atmel AVR Atmega8
 Description : 	16MHz external crystal resonator
       			resolution 1 second
 				uses HW timer2 atmega
 ============================================================================
 */


#ifndef CLOCK_H
#define CLOCK_H
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "keyboard.h"
#define ADC_DISABLE TIMSK = 0x00 //ADC Enable

typedef struct {
	volatile uint8_t second;
	volatile uint8_t minute;
	uint8_t hour;
	uint8_t day;
} time;

void timeDivision(time *tmp);
uint32_t timeToSeconds(time *tmp);
void setTime( tButton * btn, time *tmp);
void userTimer(int *turnOnTime, int *turnOffTime, void (*action1)(void), void (*action2)(void), time *tmp );
uint32_t getCurrentTime();

void goToSleep(void);
void rt_clock_init(void);
void powerSaveInit(void);

uint8_t checkPreset;
uint32_t userTurnOnTime;
uint32_t userTurnOffTime;
uint32_t saveTime;

// Variables for ISR
volatile uint8_t timeChanged;
volatile uint8_t second;
#endif
