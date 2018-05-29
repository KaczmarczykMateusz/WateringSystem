/*
 ============================================================================
 Name        : analogLightSensor.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */

#ifndef ANALOGLIGHTSENSOR_H_
#define ANALOGLIGHTSENSOR_H_

#include "..\..\include\common.h"

#define LIGHT_SENS_MAX_OUT 400; // Maximum value which may be shown by sensor (drought) multiplied "*100" minus "-1"

uint8_t lightAsPercent(void);
uint8_t lightSensor(TVOLT * voltage);

uint32_t maxValue;
uint32_t light;

#endif /* ANALOGLIGHTSENSOR_H_ */
