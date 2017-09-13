/*
 ============================================================================
 Name        : analogLightSensor.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description : Strongest light possible shows result 4.01 Volt
 	 	 	 Calibrate with vatiable'maxValue'
 ============================================================================
 */
#include "analogLightSensor.h"
/*
uint8_t lightAsPercent(void) {
	float light = adcOversampleEfficient();
	float onePercent = (float)100/8192;
	light = light * onePercent;
	return (uint8_t)light;
}*/
uint8_t lightSensor(TVOLT * voltage) {
	light = voltage->adcVoltRaw;
	maxValue = 454;
	light = (1000000/maxValue) * light;
	light /= 10000;

	if(light > 100) {
		light = 100;
	}

	return (uint8_t)light;
}
