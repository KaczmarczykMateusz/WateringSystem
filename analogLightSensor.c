/*
 ============================================================================
 Name        : analogLightSensor.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description : Strongest light possible shows result 4.01 Volt
 ============================================================================
 */

float lightAsPercent(void) {
	float light = adcOversampleEfficient();
	float onePercent = (float)100/8192;
	light = light * onePercent;
	return light;
}
