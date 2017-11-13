/*
 ============================================================================
 Name        : requestValidation.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
			   Decision taking functions for inteligent watering system project
 ============================================================================

 */
#ifndef REQUEST_VALIDATION_H
#define REQUEST_VALIDATION_H

#include <stdio.h>
#include <avr/io.h>

enum startAction {
	DISABLED			= 0,
	START_CHECK,
	WAIT_TO_VALIDATE,
	CONFIRM_VALIDATION,
	ACTION_STARTED
};

typedef struct _condSwitch {
	uint32_t turnOnTime;
	uint32_t activeTime;
	uint8_t moistureMin;
	uint16_t presetWf ;
}condSwitch;


typedef struct _value {
	uint8_t moisture;
	uint32_t temp;
	uint16_t wfVolume;
	uint8_t bright;
}value;

void (*startCallback)(void);
void (*endCallback)(void);

void registerStartActionCallback(void (*endAction)(void));
void registerEndActionCallback(void (*endAction)(void));

void updateConditionalSwitch(uint32_t, uint32_t, uint8_t, uint16_t);
void updateSensorValues(uint8_t, uint32_t, uint16_t, uint8_t);
void conditionalSwitch(uint32_t currentTime, uint8_t turnOff);


uint8_t actionExecuting;

#endif


