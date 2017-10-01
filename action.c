/*
 ============================================================================
 Name        : action.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "action.h"

void timerSetMode(void) {
	setTimerFlag = 1;
}

void exitTimerSetOnMode(void) {
	setTimerFlag = 0;
}

void exitTimerSetMode(void) {
	executedFlag = 0;
}

void relOFF(void) {
	RELAY_OFF;
}

void relON(void) {
	RELAY_ON;
}

void outOFF(void) {
	ALARM_OFF;
}

void turnAlarmOn(void) {
	alarmActive = 1;
}

void turnAlarmOff(void) {
	alarmActive = 0;
}

void outAlarm(void) {
	static uint8_t alreadyOn = 0;
	if(alarmActive) {
		if(alreadyOn) {
			ALARM_ON;
			alreadyOn = 0;
		} else {
			ALARM_OFF;
			alreadyOn = 1;
		}
	} else {
		ALARM_OFF;
	}
}

void outON(void) {
	ALARM_ON;
}

void uartWriteCurrTime(void) {
	if(global.hour < 10) {
		uartPuts("0");
	}
	sendInteger(global.hour);
	uartPuts(":");
	if(global.minute < 10) {
		uartPuts("0");
	}
	sendInteger(global.minute);
	uartPuts(":");

	if(global.second < 10) {
		uartPuts("0");
	}
	sendInteger(global.second);
}

void uartWriteTemp(TEMP * temperature) {
	sendInteger(temp.tempInt);
	uartPuts(".");
	sendInteger(temp.tempFract);
	uartPuts("*C");
}

void uartWriteSensorData(void) {
	//TODO: implement function sending all sensor data
}
