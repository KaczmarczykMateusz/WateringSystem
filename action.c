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
	uartPuts("Temp. ");
	sendInteger(temp.tempInt);
	uartPuts(".");
	sendInteger(temp.tempFract);
	uartPuts("*C");
}

void uartWriteLight(uint8_t percent) {
	uartPuts("Light: ");
	sendInteger(percent);
	uartPuts("%");
}

void uartWriteMoisture(uint8_t sensorID, uint8_t *percent) {
	char moistString[30];
	for(uint8_t i=0; i < sensorID; i++) {
		sprintf(moistString, "Moisture sensor No.%d: %d%%\n\r", i, percent[i]);
		uartPuts(moistString);
	}
}

void uartWriteSensorData(void) {
	//TODO: implement function sending all sensor data
}
