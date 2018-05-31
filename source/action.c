/*
 ============================================================================
 Name        : action.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "../include/action.h"

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
void uartWriteCurrTime(time currTime) {
	if(currTime.hour < 10) {
		uartTxStr("0");
	}
	uartTxInt(currTime.hour);
	uartTxStr(":");
	if(currTime.minute < 10) {
		uartTxStr("0");
	}
	uartTxInt(currTime.minute);
	uartTxStr(":");

	if(currTime.second < 10) {
		uartTxStr("0");
	}
	uartTxInt(currTime.second);
}

/*************************************************************************
 Function:	uartWriteCurrTime()
 Input:		Temperature struct
 Purpose:	Send temperature read from single person read via UART  to console
 **************************************************************************/
void uartWriteTemp(TEMP * temperature) {
	uartTxStr("Temp. ");
	uartTxInt(temperature->tempInt);
	uartTxStr(".");
	uartTxInt(temperature->tempFract);
	uartTxStr("*C");
}

/*************************************************************************
 Function:	uartWriteLight()
 Input:		Brightness as percents of maximum read from sensor
 Purpose:	Send input data via UART  to console
 **************************************************************************/
void uartWriteLight(uint8_t percent) {
	uartTxStr("Light: ");
	uartTxInt(percent);
	uartTxStr("%");
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
		uartTxStr(moistString);
	}
}

/*************************************************************************
 Function: 	uartWriteWaterflow()
 Input:		Water flow per minute[cl/min] and it's full volume [cl]
 Purpose:	Send input data via UART  to console
 **************************************************************************/
void uartWriteWaterflow(uint32_t perMinute, uint32_t accumulated) {
		uartTxInt(accumulated);
		uartTxInt(perMinute);
}

/*************************************************************************
 Function:	uartWriteSensorData()
 Input:		temperature struct, brightness percentage, amount of moisture
 	 	 	sensors and integer array with their percentages
 Purpose:	Send input data from all sensors available + time
  	  	  	via UART  to console
 **************************************************************************/
void uartWriteSensorData(TEMP *temperature, uint8_t light, uint8_t moistSenCount, uint8_t *moistPercent, uint32_t perMinute, uint32_t accumulated, time _globalTime) {
	uartWriteCurrTime(_globalTime);
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
void menuItem(char *row1, char *row2, time *_time) {
	lockMainScreen = 1;
/*
	if(	(INPUT_MODIFIED_CHECK) ||
		(SEC_CHANGED_CHECK)) {
		printSimpleScreen(row1, row2);
		INPUT_MODIFIED_CLEAR;
		SEC_CHANGED_CLEAR;
		_time->second = second;
		timeDivision(_time);
	}
*/
}

/*************************************************************************
 Function: 	()
 Purpose:
 **************************************************************************/
void exitServiceMode(void) {
	setTimerFlag = 7;
}

/*************************************************************************
 Function: 	menuIncr()
 Purpose:
 **************************************************************************/
void menuIncr(void) {
	setTimerFlag++;
}