/*
 ============================================================================
 Name        : moistureSensor.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "moistureSensor.h"

uint32_t moistureSensorInit(void) {
	uint32_t relativeMoist;
	relativeMoist = 8192L / (8192L*8192L);
	relativeMoist /= 100;
	return (uint32_t)relativeMoist;
}

float moistureSensor(void) {
	uint32_t  releMoist = moistureSensorInit();
	float currentMoist = (float)adcOversampleEfficient();
	currentMoist /= (currentMoist*currentMoist);
/*
	if(currentMoist > MAX_MOIST) {
		currentMoist = 100;
	} else if(currentMoist < zeroMoist) {
		currentMoist = 0;
	} else {*/
		currentMoist /= releMoist;
//	}
	return currentMoist;
}

uint8_t moisture(void * moisture(void)) {
	uint8_t enableSystemOn = 0;/*
	if(moisture->sensor1 == 1) { // TODO: implement analog value instead of flags
		if(moisture->sensor2 ==1 ) {
			if(moisture->sensor3 == 1) {
				if(moisture->sensor4 == 1) {
					enableSystemOn = 1;
				}
			}
		}
	}*/
	presetWaterVolume = 2; // TODO: implement counting water volume
	return enableSystemOn;
}

