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
	sendInteger(temp.tempInt);
	uartPuts(".");
	sendInteger(temp.tempFract);
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

/*************************************************************************
 Function: incrDcr()
 Purpose:  Incrementing and/ or decrementing time or value
 Input:    Struct with adding and subtracting buttons parameters
 	 	   and struct with time to be set
 Returns:  Set if time got changed
 **************************************************************************/
uint8_t incrDcr(tButton *addBtn, tButton *subtrBtn, uint8_t *val, time *tmp) {
	static uint8_t longPress = 0;
	static uint8_t shorterDelay = 0;
	uint8_t opPerformed = 0;

	register uint8_t addPressed;

	addPressed = addBtn ? (*addBtn->K_PIN & addBtn->key_mask) : 1;
	register uint8_t subtrPressed;
	subtrPressed = subtrBtn ? (*subtrBtn->K_PIN & subtrBtn->key_mask) : 1;

	if (!(addPressed && subtrPressed)) {
		uint8_t temporaryVal = 1;
		opPerformed = 1;
		if (longPress > 30) {
			temporaryVal += 60;
		} else if (longPress > 15) {
			temporaryVal += 30;
		} else if (longPress > 0) {
			temporaryVal += 3;
		}

		if(tmp && !addPressed) {
			if(temporaryVal >= 60) {
				tmp->hour ++;
			} else {
				tmp->minute += temporaryVal;
			}
			timeDivision(tmp);
		} else if(tmp && !subtrPressed) {
			if(temporaryVal < 60) {
				tmp->minute -= temporaryVal;
			}
			if(tmp->minute > 59) {
				tmp->minute = 59;
				tmp->hour --;
				if(tmp->hour > 59) {
					tmp->hour = 23;
				}
			}
			timeDivision(tmp);
		} else if(val && !addPressed) {
			*val += temporaryVal;
		} else if(val && !subtrPressed) {
			*val -= temporaryVal;
		}

		if(!shorterDelay) {
			_delay_ms(120);
		}
		_delay_ms(120);
		if (!(addPressed && subtrPressed)) {
			shorterDelay = 1;
			if (longPress <= 26) {
				longPress++;
			}
		}
		SEC_CHANGED_SET;
	} else {
		longPress = 0;
		shorterDelay = 0;
	}

	if(!opPerformed) {
		return 0;
	} else {
		return 1;
	}
}

