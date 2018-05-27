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

#include "adc.h"
#include "dataParsing.h"
#include "common.h"

#define MOIST_SENSORS_NUMBER 2

#define MOIST_SENSOR_POWER_MASK	(1 << 0)

uint8_t sensorNumber;


/** @brief	: Process actual ADC conversion using functions directly linked with hardware
 *  @param  *voltage : Structure holding all variables related to ADC results
 *  @return	: Moisture as percent of maximum possible read
 */
uint8_t moistureSensor(TVOLT * voltage);

/** @brief	Powers down single sensor or group of them connected to single PIN
 *  @param  *S_DDR		: Address of DDR of powering moisture sensor
 *  @param  *S_PORT		: Address of PORT of powering moisture sensor
 *  @param  sensorMask	: Bit mask of PIN powering moisture sensor
 *  @return	: None
 */
void moistSensPwrDwn(volatile uint8_t *S_DDR, volatile uint8_t *S_PORT, uint8_t sensorMask);

/** @brief	Powers up single sensor or group of them connected to single PIN
 *  @param  *S_DDR		: Address of DDR of powering moisture sensor
 *  @param  *S_PORT		: Address of PORT of powering moisture sensor
 *  @param  sensorMask	: Bit mask of PIN powering moisture sensor
 *  @return	: None
 */
void moistSensPwrUp(volatile uint8_t *S_DDR, volatile uint8_t *S_PORT, uint8_t sensorMask);

/** @brief	Powers up moisture sensors in order to prepare them to conversion by moistCheckResult()
 *  @param  sensorMask	: Bit mask of PIN powering moisture sensors
 *  @return	: Flag confirming that there are no more sensors to check
 *  @see	: moistCheckStart()
 */
uint8_t moistCheckStart(uint8_t sensNo);

/** @brief	Process ADC conversion from start till the end and convert result into percent via moistureSensor()
 * 			All happens only if sensors are successfully powered by moistCheckStart()
 *  @param  flag	: Flag enabling mesurnment which should be set by moistCheckStart()
 *  @param  *mSens	: Structure holding all variables related to ADC results
 *  @param  *sensNo	: Number of sensor being checked, function is allowed to increment this number

 *  @return	: Moisture as percent of maximum possible read
 *  @see	: moistCheckStart() which sets flag flag
 *  		  moistureSensor() which actually reads and converts result
 */
uint8_t moistCheckResult(uint8_t flag, TVOLT *mSens, uint8_t *sensNo) ;

/** @brief	Set minimum and maximum  values which may be read from sensor
 * 			while out of substrate and in the water
 *  @param  enableServiceMode	: Flag enabling entire function
 *  @param  menuIndex			: Index of menu step
 *  @param  *addBtn				: Address of struct with consisting button parameters
 *  							  (which role is to increase value) or NULL
 *  @param  *subtrBtn			: Address of struct with consisting button parameters
 *  							  (which role is to decrease value) or NULL
 *  @return	: 1 : Service MENU displayed
 *  		  0 : Service MENU not displayed
 */
//uint8_t calibrateMoistSensor(uint8_t enableServiceMode, uint8_t menuIndex, uint8_t actualMoist, tButton *addBtn, tButton *subtrBtn, time _globalTime);

uint16_t moistSensMinOut; //@brief: Value in volt multiplied "* 100"
uint16_t moistSensMaxOut; //@brief: Maximum value which may be shown by sensor (drought) multiplied "*100" minus "-1"



//TODO: implement it
typedef struct {
	uint8_t moistSensorsFlags:6;	// it is simultaneously value of PIN from which sensor is powered !
}moistSensorsFlags;


#endif
