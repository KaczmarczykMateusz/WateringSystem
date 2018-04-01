/*
 ============================================================================
 Name        : action.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "action.h"


// Outputs definitions
#define RELAY_INIT 		DDRC |= (1<<PC0)
#define RELAY_ON    	PORTC &= ~(1<<PC0)
#define RELAY_OFF  		 PORTC |= (1<<PC0)
#define RELAY_TOGGLE    PORTC ^= (1<<PC0)
#define IS_RELAY_ON	    PORTC & (1<<PC0)

#define ALARM_INIT 	DDRC |= (1<<PC1)			/// Implement it in main instead of OUTPUT_PIN_A111
#define ALARM_ON    PORTC &= ~(1<<PC1)
#define ALARM_OFF   PORTC |= (1<<PC1)
#define ALARM_TOGGLE    PORTC ^= (1<<PC1)
#define IS_ALARM_ON	    PORTC & (1<<PC1)

void relOFF(void) {
	RELAY_OFF;
}

void relON(void) {
	RELAY_ON;
}

void relToggle(void) { // TODO: implement in instead of outON/outOFF
	RELAY_TOGGLE;
}

void outON(void) {
	ALARM_ON;
}

void outOFF(void) {
	ALARM_OFF;
}

void outToggle(void) { // TODO: implement in instead of outON/outOFF
	ALARM_TOGGLE;
}

void isOutOn(void) {
	alarmActive = 1;
}

void turnAlarmOn(void) {
	alarmActive = 1;
}

void turnAlarmOff(void) {
	alarmActive = 0;
}

void outAlarm(void) {
	if(alarmActive) {
		ALARM_TOGGLE;
	} else {
		ALARM_OFF;
	}
}

/*************************************************************************
 Function:	uartWriteCurrTime()
 Purpose:	Write current real time via UART  to console
 **************************************************************************/
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

/*************************************************************************
 Function:	uartWriteCurrTime()
 Input:		Temperature struct
 Purpose:	Send temperature read from single person read via UART  to console
 **************************************************************************/
void uartWriteTemp(TEMP * temperature) {
	uartPuts("Temp. ");
	sendInteger(temperature->tempInt);
	uartPuts(".");
	sendInteger(temperature->tempFract);
	uartPuts("*C");
}

/*************************************************************************
 Function:	uartWriteLight()
 Input:		Brightness as percents of maximum read from sensor
 Purpose:	Send input data via UART  to console
 **************************************************************************/
void uartWriteLight(uint8_t percent) {
	uartPuts("Light: ");
	sendInteger(percent);
	uartPuts("%");
}

/*************************************************************************
 Function:	uartWriteMoisture()
 Input:		Amount of sensors and their reads as pointer to integer array
 Purpose:	Send percentage of moisture read from multiple sensors
 	 	 	 via UART  to console
 **************************************************************************/
void uartWriteMoisture(uint8_t sensorCount, uint8_t *percent) {
	char moistString[30];
	for(uint8_t i=0; i < sensorCount; i++) {
		sprintf(moistString, "Moisture sensor No.%d: %d%%\n\r", i, percent[i]);
		uartPuts(moistString);
	}
}

/*************************************************************************
 Function: 	uartWriteWaterflow()
 Input:		Water flow per minute[cl/min] and it's full volume [cl]
 Purpose:	Send input data via UART  to console
 **************************************************************************/
void uartWriteWaterflow(uint32_t perMinute, uint32_t accumulated) {
		sendInteger(accumulated);
		sendInteger(perMinute);
}

/*************************************************************************
 Function:	uartWriteSensorData()
 Input:		temperature struct, brightness percentage, amount of moisture
 	 	 	sensors and integer array with their percentages
 Purpose:	Send input data from all sensors available + time
  	  	  	via UART  to console
 **************************************************************************/
void uartWriteSensorData(TEMP *temperature, uint8_t light, uint8_t moistSenCount, uint8_t *moistPercent, uint32_t perMinute, uint32_t accumulated) {
	uartWriteCurrTime();
	uartWriteTemp(temperature);
	uartWriteLight(light);
	uartWriteMoisture(moistSenCount, moistPercent); // TODO check if pointer working properly
	uartWriteWaterflow(perMinute, accumulated);
}

void serviceModeEntry(void) {
	enbleSrvceMode ^= (1 << 0);
	/*
	if(!enbleSrvceMode) {
		enbleSrvceMode = 1;
	} else {
		enbleSrvceMode = 0;
	}
	*/
}

/*************************************************************************
 Function: menuItem()
 Purpose: Print text, update global time and clear flags menu and RTC flags
 Input:	Two rows of text as max 16 char arrays (each one)
 **************************************************************************/
void menuItem(char *row1, char *row2) {
	lockMainScreen = 1;

	if(	(INPUT_MODIFIED_CHECK) ||
		(SEC_CHANGED_CHECK)) {
		printSimpleScreen(row1, row2);
		INPUT_MODIFIED_CLEAR;
		SEC_CHANGED_CLEAR;
		global.second = second;
		timeDivision(&global);
	}
}

/*************************************************************************
 Function: 	()
 Purpose:
 **************************************************************************/
void exitServiceMode(void) {
	setTimerFlag = 7;
}

/*************************************************************************
 Function: 	incrDigit()
 Purpose:
 **************************************************************************/
void incrDigit(void) {
	setTimerFlag++;
}
