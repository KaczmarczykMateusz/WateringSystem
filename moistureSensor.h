/*
 ============================================================================
 Name        : moistureSensor.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description : Higher sensor value means dryer soil
 	 	 	 although result is presented with "correct" percentage
 ============================================================================
 */
#ifndef MOISTURE_SENSOR_H
#define MOISTURE_SENSOR_H
#define MAX_MOIST 100
#define MIN_MOIST 0

// Outputs definitions
//#define MOISR_SENSORS_INIT DDRC |= 0x1F

#include <stdio.h>
#include <util/delay.h>
#include <math.h>

#include "lcd.h"
#include "adc.h"
#include "keyboard.h"
#include "dataParsing.h"

#define MOIST_SENSORS_NUMBER 2

char bufferLCD[16];

uint8_t sensorNumber;

float measureMoisture(void);
void moistSensPwrDwn(volatile uint8_t *S_DDR, volatile uint8_t *S_PORT, uint8_t sensorMask);
void moistSensPwrUp(volatile uint8_t *S_DDR, volatile uint8_t *S_PORT, uint8_t sensorMask);
void moistCheckStart(uint8_t *flag, uint8_t *sensNo);
void moistCheckResult(uint8_t flag, TVOLT *mSens, uint8_t *sensNo, uint8_t *result) ;

void checkMoistSensor(void);
void calibrateMoistSensor(tButton * setBtn, tButton * selectBtn);
uint8_t writeMoisture(volatile uint8_t * KPIN);
void moisturePrintOnLCD(void);
void choiceOfMenu(void);
void incrementMinVal(void);
void incrementMaxVal(void);
uint8_t moistureSensor(TVOLT * voltage);

int32_t moistResult;
uint16_t moistSensMinOut; // value in volt multiplied "* 100"
uint16_t moistSensMaxOut; // Maximum value which may be shown by sensor (drought) multiplied "*100" minus "-1"
uint8_t moistSensors[6];
uint8_t choseMenu;

//uint8_t moisture(void * (void));
uint32_t tempMoist;

// it is simultaneously value of PIN from which sensor is powered !
typedef struct {
	uint8_t moistSensorsFlags:6;
}moistSensorsFlags;


#endif
