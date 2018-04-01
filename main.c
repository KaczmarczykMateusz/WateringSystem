
//main.h ---- delete enums








/*
 ============================================================================
 Name        : main.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "main.h"
#include <math.h>

char* sysNotReadyBuff = "CZEKA";
char* sysReadylBuff = "GOTOWY";
char* sysWorkingBuff = "PRACA";
char* litreCtrlTxt = "ltr";
char* minuteCtrlTxt = "min";
char* moistOnTxt = "TAK";
char* moistOffTxt = "NIE";

char* currCtrlBuff;
char* currMoistCtrlBuff;
char* currSysStatusBuff;


int main(void) {
	uint8_t dsFlag = 0;
	uint8_t minMoist = 0;
	uint32_t wfDoseVol = 0;
	uint8_t temporaryLockFlag = 1;

	uint32_t wfVolAccu = 0;
	uint32_t wfTemp = 0;

	uint8_t resetSecond = 0;
	uint32_t timerTime = 0;

	uint16_t blinkDelay;	//TODO: delete global definition
	uint8_t lightStrength = 0;

	activateSystem = 0;

//TODO: implement macro defining button port letter (eg. "A", or "B")
	//TODO: add btn waking up display(stopBtn)
	tButton stopBtn = btnInit(&DDRD, &PORTD, &PIND, STOP_BTN_MASK);
	tButton selectBtn = btnInit(&DDRD, &PORTD, &PIND, SELECT_BTN_MASK);
// TODO: change buttons masks (probablu to "override" adc)
	tButton incrTimeBtn = btnInit(&DDRD, &PORTD, &PIND, INCR_BTN_MASK);
	tButton dcrTimeBtn = btnInit(&DDRA, &PORTA, &PINA, DCR_BTN_MASK);

	condSwitch switchConditions; // todo: throw it out from header file and define here
	value val;
	TEMP temp;
    time activeTime;
	status systemStatus;

	TVOLT light;
	light.ref_v = 234;
	light.ref_adc = 2100;

	TVOLT moist[3];
	moist[0].ref_v = 234;
	moist[0].ref_adc = 2100;

	moist[1].ref_v = 234;
	moist[1].ref_adc = 2100;

	blinkDelay = BLINK_DELAY_FREQ;
	controlFactor = LITRES;
	systemStatus = NOT_READY;
	moistCtrl = MOIST_OFF;

	systemInit();

	sprintf(firstRowBuffLCD,"INITIALISATION ");
	sprintf(secondRowBuffLCD,"INITIALISATION");
	printSimpleScreen(firstRowBuffLCD, secondRowBuffLCD);
	_delay_ms(500);
	while (1) {
// 		@brief: stops alarm or watering ONLY while it's running

		if(!setTimerFlag) {
			keyLongPress(&selectBtn, NULL, incrDigit);
		}

		uartCheck(parse_uart_data);

		//TODO: move it to some function
		if(  (controlFactor==MINUTES) && (currCtrlBuff!=minuteCtrlTxt)  ) {
			currCtrlBuff= minuteCtrlTxt;
		} else if(  (controlFactor==LITRES) && (currCtrlBuff!=litreCtrlTxt)  ) {
			currCtrlBuff= litreCtrlTxt;
		}
		if(  (moistCtrl==MOIST_ON) && (currMoistCtrlBuff!=&moistOnTxt[0])  ) {
			currMoistCtrlBuff= moistOnTxt;
		} else if(  (moistCtrl==MOIST_OFF) && (currMoistCtrlBuff!=moistOffTxt)  ) {
			currMoistCtrlBuff= moistOffTxt;
		}
		if(  (systemStatus==READY) && (currSysStatusBuff!=&sysReadylBuff[0])  ) {
			currSysStatusBuff= sysReadylBuff;
		} else if(  (systemStatus==WORK) && (currSysStatusBuff!=&sysWorkingBuff[0])  ) {
			currSysStatusBuff= sysWorkingBuff;
		}else if(  (systemStatus==NOT_READY) && (currSysStatusBuff!=&sysNotReadyBuff[0])  ) {
			currSysStatusBuff= sysNotReadyBuff;
		}

        if(  (SEC_CHANGED_CHECK) ) {
        	SEC_CHANGED_CLEAR;

	        if(MIN_CHANGED_CHECK) {
				// TODO take the lowest or avarage moisture and pass it timerWithMoist (which doesn't exist)
				moisture[sensorNumber] = moistCheckResult(temporaryLockFlag, moist ,&sensorNumber);
	        	temporaryLockFlag = moistCheckStart(sensorNumber);
	        	if(!temporaryLockFlag) {
	        		MIN_CHANGED_CLEAR;
	        	}
#if 0
	        	voltAdc(adcOversample(0x07, 3), &light); // replace this name with adcConvert(adcOversample(0x07, 3), &light);
		        // TODO: implement pwrUp and pwrDown even though we can check brightness constantly there is no need so better save power
				lightStrength = lightSensor(&light);
#endif
	        }
			if(!dsFlag) {			//	Start conversion
				ds18b20_ConvertT();
				dsFlag = 1;
			} else {				// Read converted temperature
				readTemperature(&temp);
				sprintf(currTemp, "%2ld.%01ld\xdf""C", temp.tempInt, temp.tempFract);
				dsFlag = 0;
			}
			val = updateSensorValues(moisture[0], temp.tempMultip, wfVolAccu, lightStrength);
			switchConditions = updateConditionalSwitch(timeToSeconds(&turnOnTime), 100, minMoist, wfDoseVol, timeToSeconds(&activeTime));//TODO replace hardcoded val
			systemStatus = conditionalSwitch(switchConditions, val, timeToSeconds(&global), activateSystem);

        	if(resetSecond == 1) {
        		second = 0;
        	}
        	global.second = second;
        	timeDivision(&global);
			if(	(!(wfStatus & WF_COUNT_RUNNING)) &&
					wfTemp) {
				wfVolAccu += wfTemp / 60;
			}
			wfSensToggle(  ( (systemStatus==WORK)&&(controlFactor==LITRES) )? 1: 0  );

			if(!setTimerFlag) {
				printMainScreen(turnOnTime, timerTime, global, currMoistCtrlBuff, currSysStatusBuff);
			}
#if 0	//TODO: probably we won't need it anymore but let's keep it until conditionalSwitch tested
        	if(controlFactor == MINUTES) {
        		systemStatus = timerSwitch(timeToMinutes(&global), timeToMinutes(&turnOnTime), timerTime )?WORK:READY;
        	} else {
        		systemStatus = READY;//TODDO: implement right function
        	}
#endif
#if 0
	        //outAlarm();
	        //turnAlarmOff();

			uartPuts("\f");
			uartWriteCurrTime();
			uartPuts("\n\r");
			uartWriteTemp(&temp);
			uartPuts("\n\r");
			uartWriteLight(lightStrength);
			uartPuts("\n\r");
			uartWriteMoisture(MOIST_SENSORS_NUMBER, moisture);

			//Waterflow measurnment block TODO: optimize code below and linked t it
			char uartBuff[16];
			uartPuts("\n\rwf vol accumulated= ");
			sprintf(uartBuff, "%ld", wfVolAccu); //TODO, check if this solves issue of too short result
			uartPuts(uartBuff);
			uartPuts("\n\rMeasureWF() = ");
			wfTemp = measureWF();
			sprintf(uartBuff, "%ld", wfTemp); //TODO, check if this solves issue of too short result
			uartPuts(uartBuff);
#endif
        }



//	****************** MENU ******************	//
        static uint8_t isBlink = 0;	//TODO:remove global declaration
		if((BLINK_DELAY_FREQ/2) == blinkDelay--) {
			printMainScreen(turnOnTime, timerTime, global, currMoistCtrlBuff, currSysStatusBuff);
		} else if((BLINK_DELAY_FREQ/2) > blinkDelay) {

		} else if(BLINK_DELAY_FREQ < blinkDelay) {
			blinkDelay = BLINK_DELAY_FREQ;
			isBlink = 1;
		}
		static uint32_t lengthON = 0;


		switch(setTimerFlag) {

		case 1: //Set system clock
			if(isBlink) {
				sprintf(firstRowBuffLCD,"%02d:%02d*%3ld%s*%s", turnOnTime.hour, turnOnTime.minute,
															timerTime, currCtrlBuff, currMoistCtrlBuff);
				sprintf(secondRowBuffLCD,"  :  *%s", currSysStatusBuff);
			}
			resetSecond = incrDcr(&incrTimeBtn, &dcrTimeBtn, 0, 0, &global);//TODO: check if reset second working
			keyPress(&stopBtn, exitServiceMode);
			keyPress(&selectBtn, incrDigit);
		break;

		case 2: //Set turn ON time
			incrDcr(&incrTimeBtn, &dcrTimeBtn, 0, 0, &turnOnTime);
			keyPress(&stopBtn, exitServiceMode);
			keyPress(&selectBtn, incrDigit);
			if(isBlink) {
				sprintf(firstRowBuffLCD,"  :  *%3ld%s*%s", timerTime, currCtrlBuff, currMoistCtrlBuff);
				sprintf(secondRowBuffLCD,"%02d:%02d*%s", global.hour, global.minute, currSysStatusBuff);
			}
		break;

		case 3:
			keyLongPress(&stopBtn, NULL, exitServiceMode);
			keyPress(&incrTimeBtn, toggleCtrl);
			keyPress(&dcrTimeBtn, toggleCtrl);
			keyPress(&selectBtn, incrDigit);
			if(isBlink) {
				sprintf(firstRowBuffLCD,"%02d:%02d*%3ld   *%s", turnOnTime.hour, turnOnTime.minute,
																			timerTime, currMoistCtrlBuff);
				sprintf(secondRowBuffLCD,"%02d:%02d*%s", global.hour, global.minute, currSysStatusBuff);
			}
		break;

		case 4: //Set how long to keep ON ( (by time (minutes)||(by volume (litres) )
			incrDcr(&incrTimeBtn, &dcrTimeBtn, &lengthON, 999, NULL);
			keyPress(&stopBtn, exitServiceMode);
			keyPress(&selectBtn, incrDigit);
			if(controlFactor == MINUTES) {
				timerTime = lengthON;
				wfDoseVol = 0;
			} else {
				wfDoseVol = (lengthON*10); //multiply [l] into [dl]
				timerTime = 0;
			}
			if(isBlink) {
				sprintf(firstRowBuffLCD,"%02d:%02d*   %s*%s", turnOnTime.hour, turnOnTime.minute,
																		currCtrlBuff, currMoistCtrlBuff);
				sprintf(secondRowBuffLCD,"%02d:%02d*%s", global.hour, global.minute, currSysStatusBuff);
			}
		break;

		case 5: //Set how long to keep on (by volume (litres))
			keyLongPress(&stopBtn, NULL, exitServiceMode);	//TODO: change workStatus performed by function
			keyPress(&incrTimeBtn, toggleHumidityCtrl);
			keyPress(&dcrTimeBtn, toggleHumidityCtrl);
			keyPress(&selectBtn, incrDigit);
			if(isBlink) {
				sprintf(firstRowBuffLCD,"%02d:%02d*%3ld%s*   ", turnOnTime.hour, turnOnTime.minute, timerTime, currCtrlBuff);
				sprintf(secondRowBuffLCD,"%02d:%02d*%s", global.hour, global.minute, currSysStatusBuff);
			}
		break;

		case 6:
			keyPress(&incrTimeBtn, activDeactivSystem);
			keyPress(&dcrTimeBtn, activDeactivSystem);
			keyPress(&selectBtn, incrDigit);
			if(isBlink) {
				sprintf(firstRowBuffLCD,"%02d:%02d*%3ld%s*%s", turnOnTime.hour, turnOnTime.minute,
															timerTime, currCtrlBuff, currMoistCtrlBuff);
				sprintf(secondRowBuffLCD,"%02d:%02d*", global.hour, global.minute);
			}
		break;

		case 7:
			registerStartActionCallback(relON);
			registerEndActionCallback(relOFF);
			setTimerFlag = 0;
		break;

		default:
			if(setTimerFlag) {
				setTimerFlag = 0;
			}
		}

		if(isBlink) {
			printSimpleScreen(firstRowBuffLCD, secondRowBuffLCD);
			isBlink = 0;
		}
//      goToSleep();
	}
}

/*************************************************************************
 Function: 	systemInit()
 Purpose:	Initialise all variables and peripherals
 	 	 	need for work of the system
 **************************************************************************/
void systemInit(void) {
	ALARM_INIT;
	RELAY_INIT;

	ALARM_OFF;
	relOFF();
	outOFF();

	//	lockMainScreen = 0; TODO: remove definition of this variable
//	powerSaveInit();
	rt_clock_init();
	LCD_Initalize();
	adcInit();

	uartInit( UART_BAUD_SELECT(115200, 16000000L) );
	uartFlush();

	MIN_CHANGED_SET;
	turnAlarmOff();

	sensorNumber  = 0;
	wfStatus = 0;
}

/*************************************************************************
 Function: 	printMainScreen()
 Purpose:	Prints basic screen
 Input	:	complexCheckTime - how long to wait from start tilll the end of workStatus
 	 	 	turnOnClock	-
 	 	 	globalClock
 **************************************************************************/
void printMainScreen(time turnOnClock, uint32_t complexCheckTime, time globalClock, char* sysMoistStat, char* sysStat) {
	sprintf(firstRowBuffLCD,"%02d:%02d*%3ld%s*%s", turnOnClock.hour, turnOnClock.minute,
													complexCheckTime, currCtrlBuff, sysMoistStat);
	sprintf(secondRowBuffLCD,"%02d:%02d*%s", globalClock.hour, globalClock.minute, sysStat);
	printSimpleScreen(firstRowBuffLCD, secondRowBuffLCD);
}

/*************************************************************************
 Function: 	toggleCtrl()
 Purpose:	Toggle between system control modes
 **************************************************************************/
void toggleCtrl(void) {
	if(controlFactor == LITRES) {
		controlFactor = MINUTES;
	} else {
		controlFactor = LITRES;
	}
}

/*************************************************************************
 Function: 	toggleHumidityCtrl()
 Purpose:	Toggle between system  reacting for moisture or not
 **************************************************************************/
void toggleHumidityCtrl(void) {
	if(moistCtrl == HUMIDITY) {
		moistCtrl = SIMPLE;
	} else {
		moistCtrl = HUMIDITY;
	}
}


void activDeactivSystem(void) {
	if(activateSystem) {
		activateSystem = 0;
	} else {
		activateSystem = 1;
	}
}
