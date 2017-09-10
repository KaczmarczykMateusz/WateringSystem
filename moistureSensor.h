/*
 ============================================================================
 Name        : moistureSensor.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#ifndef MOISTURE_SENSOR_H
#define MOISTURE_SENSOR_H
#define MAX_MOIST 100
#define MIN_MOIST 0

// Outputs definitions
#define MOISR_SENSORS_INIT DDRC |= 0x1F

#include <stdio.h>
#include <util/delay.h>
#include <math.h>

#include "lcd.h"
#include "adc.h"
#include "keyboard.h"

char bufferLCD[16];
char bufferLCD_2[16];
float measureMoisture(void);
void checkMoistSensor(void);
void calibrateMoistSensor(tButton * setBtn, tButton * selectBtn);
uint8_t writeMoisture(volatile uint8_t * KPIN);
void moisturePrintOnLCD(void);
void choiceOfMenu(void);
void incrementMinVal(void);
void incrementMaxVal(void);

uint8_t moistSensors[6];
uint8_t choseMenu;
uint8_t maxMoist;
uint8_t minMoist;
uint8_t moisture(void * (void));


// it is simultaneously value of PIN from which sensor is powered !
typedef struct {
	uint8_t moistSensorsFlags:6;
}moistSensorsFlags;


#endif
