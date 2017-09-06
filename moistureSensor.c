/*
 ============================================================================
 Name        : moistureSensor.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "moistureSensor.h"

void moistureSensorInit(void) {
	fullMoist = 0;
	zeroMoist = maxAdc / (maxAdc*maxAdc);
	relativeMoist = fullMoist - zeroMoist;
	relativeMoist /= 100;
}

void moistureSensor(uint16_t currentMoist) {
	currentMoist /= (currentMoist*currentMoist);

	if(currentMoist > fullMoist) {
		currentMoist = 100;
	} else if(currentMoist < zeroMoist) {
		currentMoist = 0;
	} else {
		currentMoist /= relativeMoist;
	}
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

