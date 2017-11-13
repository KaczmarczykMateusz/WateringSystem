/*
 ============================================================================
 Name        : action.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#ifndef ACTION_H_
#define ACTION_H_

#include "main.h"

uint8_t setTimerFlag;
uint8_t executedFlag;
uint8_t alarmActive;

uint8_t lockMainScreen;


void turnAlarmOff(void);
void turnAlarmOn(void);
void timerSetModeNextStep(void);
void relOFF(void);
void relON(void);
void outOFF(void);
void outON(void);
void outAlarm(void);

void incrMinMoist(void);

void uartWriteCurrTime(void);
void uartWriteTemp(TEMP * temperature);
void uartWriteLight(uint8_t percent);
void uartWriteMoisture(uint8_t sensorID, uint8_t *percent);
void uartWriteSensorData(void);
void uartWriteWaterflow(uint32_t perMinute, uint32_t accumulated);

void menuItem(uint8_t *executedFlag, time *currentTime, tButton * btn,  char *buff);

#endif
