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
#define MAX_MOIST 0

#include <stdio.h>
#include "adc.h"

uint32_t moistureSensorInit(void);
float moistureSensor(void);

uint16_t zeroMoist;
uint8_t moisture(void * (void));

uint8_t presetWaterVolume;

#endif
