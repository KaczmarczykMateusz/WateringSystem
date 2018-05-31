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
#include "../drivers/lcd.h"
#include "../drivers/sensors/ds18b20.h"
#include "../drivers/sensors/waterflowSensor.h"
#include "../drivers/sensors/moistureSensor.h"
#include "../drivers/sensors/analogLightSensor.h"
#include "../drivers/keyboard.h"
#include "../drivers/communication/uart.h"
#include "../drivers/communication/spi.h"
#include "../drivers/adc.h"
#include "../logger/logger.h"
#include "clock.h"
#include "requestValidation.h"
#include "dataParsing.h"
#include "action.h"
#include "common.h"

static const uint16_t BLINK_DELAY_FREQ = 17500;

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
void printMainScreen(time turnOnClock, uint32_t ctrlVar, time globalClock, char* sysMoistStat,  char*  currCtrlBuff, char* sysStat);

void toggleCtrl(void);
void activDeactivSystem(void);
TEMP readTemp(void);

value initSensVal();
condSwitch initCondSwitch();

#endif
