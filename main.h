/*
 ============================================================================
 Name        : main.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#ifndef MAIN_H_
#define MAIN_H_

// CPU 16MHz
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>
#include "lcd.h"
#include "ds18b20.h"
#include "clock.h"
#include "keyboard.h"
#include "moistureSensor.h"
#include "requestValidation.h"
#include "waterflowSensor.h"
#include "adc.h"
#include "analogLightSensor.h"
#include "uart.h"
#include "dataParsing.h"
#include "action.h"
#include "spi.h"
#include "fat16.h"

static const uint16_t BLINK_DELAY_FREQ = 35000;
uint16_t blinkDelay;

time global;
time turnOnTime;
time activeTime;

TEMP temp;

double wf;
char printLCDBuffer[16];
char buff1[16];

char currTemp[9];
uint8_t lightStrength;
uint8_t moisture[3];

uint16_t blinkDelays;
char firstRowBuffLCD[17];
char secondRowBuffLCD[17];





typedef enum {
	READY,
	WORK
} status;

typedef enum {
	MINUTES,
	LITRES
} control;

status systemStatus;
control controlFactor;

void setClockMode(void);
void setTimeOnMode(void);
void setTimerMode(void);
void setVolumeMode(void);
void setCtrlMode(void);
void setHumidityMode(void);
void exitServiceMode(void);

/**
 * @brief:	Initialise all variables and peripherals
 * 			need for work of the system
 */
void systemInit(void);
void printMainScreen(void);
void controlMode(void);

#endif
