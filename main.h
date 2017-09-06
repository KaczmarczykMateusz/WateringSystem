/*
 ============================================================================
 Name        : main.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
// Outputs definitions
#define RELAY_INIT DDRC |= (1<<PC0)
#define RELAY_ON    PORTC &= ~(1<<PC0)
#define RELAY_OFF   PORTC |= (1<<PC0)

#define OUTPUT_PIN_A111_INIT DDRC |= (1<<PC1)
#define OUTPUT_PIN_A111_ON    PORTC &= ~(1<<PC1)
#define OUTPUT_PIN_A111_OFF   PORTC |= (1<<PC1)

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

double wf;
char printLCDBuffer[50];
double temp;
unsigned char ds18b20_pad[9];
char str[8];


void relOFF(void);
void relON(void);
void outOFF(void);
void outON(void);
