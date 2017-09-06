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

#include <stdio.h>
#include "adc.h"

void moistureSensorInit(void);
void moistureSensor(uint16_t currentMoist);

uint16_t relativeMoist;
uint16_t fullMoist;
uint8_t zeroMoist;
uint8_t currentMoisture;
uint8_t moisture(void * (void));

uint8_t presetWaterVolume;

#endif
