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
#include "common.h"
#include "logger/logger.h"

static const uint16_t BLINK_DELAY_FREQ = 35000;

TEMP temp;

double wf;

char firstRowBuffLCD[17];
char secondRowBuffLCD[17];


status activateSystem;

control controlFactor;
moistureCtrl moistCtrl;


void exitServiceMode(void);

/**
 * @brief:	Initialise all variables and peripherals
 * 			need for work of the system
 */
void systemInit(void);
void printMainScreen(time turnOnClock, uint32_t timer, time globalClock, char* sysMoistStat,  char*  currCtrlBuff, char* sysStat);

void toggleCtrl(void);
void activDeactivSystem(void);
TEMP readTemp(void);

value initSensVal();
condSwitch initCondSwitch();

#endif
