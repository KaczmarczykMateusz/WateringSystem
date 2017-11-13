/*
 ============================================================================
 Name        : action.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "action.h"

void timerSetModeNextStep(void) {
	setTimerFlag++;
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

void uartWriteWaterflow(uint32_t perMinute, uint32_t accumulated) {
		sendInteger(accumulated);
		sendInteger(perMinute);
}

void uartWriteSensorData(void) {
	//TODO: implement function sending all sensor data
}


void menuItem(uint8_t *executedFlag, time *actionTime, tButton * btn, char *buff) {
	lockMainScreen = 1;
	if(*executedFlag > 100) {
		*executedFlag = 100;
	}
	if(SEC_CHANGED_CHECK) {
		LCD_Clear();
		LCD_WriteText(buff);
		if(actionTime) {
			sprintf(buff,"%02d:%02d:%02d", actionTime->hour, actionTime->minute, actionTime->second);
		} else if(executedFlag){
			sprintf(buff,"%d%%", *executedFlag);
		}
		LCD_GoTo(0,1);
		LCD_WriteText(buff);

		SEC_CHANGED_CLEAR;
		global.second = second;
		timeDivision(&global);
	}
}
