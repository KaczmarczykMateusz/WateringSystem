/*
 ============================================================================
 Name        : WaterFlowSensor.h
 Author      : Mateusz Kaczmarczyk
 Version     : Waterflow sensor YF-S201 with AVR Atmega
 Description : External quartz 16MHz
 ============================================================================
*/
#ifndef WATERFLOESENSOR_H
#define WATERFLOESENSOR_H
#include <avr/interrupt.h>

double WaterFlow(void);
double trigger(void);

volatile uint32_t waterflowAux;
float waterflow;

#endif
