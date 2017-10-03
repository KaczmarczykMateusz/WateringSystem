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

// Outputs definitions
#define RELAY_INIT DDRC |= (1<<PC0)
#define RELAY_ON    PORTC &= ~(1<<PC0)
#define RELAY_OFF   PORTC |= (1<<PC0)

#define OUTPUT_PIN_A111_INIT DDRC |= (1<<PC1)
#define ALARM_ON    PORTC &= ~(1<<PC1)
#define ALARM_OFF   PORTC |= (1<<PC1)

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

time global;
time turnOnTime;
time turnOffTime;

TEMP temp;

double wf;
char printLCDBuffer[50];

char currTemp[9];
uint8_t lightStrength;
uint8_t moisture[3];

#endif
