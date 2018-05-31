//TODO: consider removing menuItem() from action.c
//TODO: try to change keyLongPress that it performs only long press OR short press action
//TODO: add sleep mode and int to btn waking up display(stopBtn)
//TODO: try to swipe logic in incrDcr and btnPressed
/*
 ============================================================================
 Name        : main.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "include/main.h"
#include <math.h>

#define LOGER_ACTIVE 		0
#define LIGHT_SENS_ACTIVE	0
#define MOIST_SENS_ACTIVE	0
#define MAIN_DEBUG_ACTIVE	1
#define WF_SENS_DEBUG		1
#define MOIST_CONTROL_DEFAULT	MOIST_CHECK_OFF
#define CONTROL_FACTOR_DEFAULT	MINUTES

/*
#define LIGHT_SENSOR_PIN 0x08
#define MOIST_SENSOR_1_PIN 0x01
#define MOIST_SENSOR_2_PIN 0x02
*/
int main(void) {
#if LOGER_ACTIVE
	uint8_t loggerStatus = LOGGER_NOT_READY;
#endif
#if LIGHT_SENS_ACTIVE
	TVOLT light = initAdcStruct(234, 2100);
#endif
	char* currCtrlBuff = '\0';
	char* currMoistCtrlBuff = '\0';
	char* currSysStatusBuff = '\0';

	time global = initTime();
	time turnOnTime = initTime();
//	time activeTime = initTime(); //XXX: replaced by

	TEMP temp;
	value _sensVal = initSensVal();

	condSwitch switchConditions = initCondSwitch();
	controlFactor = switchConditions.ctrlFactor;

	status systemStatus = NOT_READY;

	uint8_t moisture[2];
	moisture[0] = 0;
	moisture[1] = 0;

	uint32_t wfDoseVol = 0;
	uint8_t resetSecond = 0;
	uint32_t timerTime = 0;

	uint32_t litreTime = 0;
	actionExecuted = 0;

	uint16_t blinkDelay = BLINK_DELAY_FREQ;
	uint8_t lightStrength = 0;
	activateSystem = NOT_READY;

	tButton stopBtn = btnInit(&BUTTON_DDR, &BUTTON_PORT, &BUTTON_PIN, STOP_BTN_MASK);
	tButton selectBtn = btnInit(&BUTTON_DDR, &BUTTON_PORT, &BUTTON_PIN, SELECT_BTN_MASK);
	tButton incrTimeBtn = btnInit(&BUTTON_DDR, &BUTTON_PORT, &BUTTON_PIN, INCR_BTN_MASK);
	tButton dcrTimeBtn = btnInit(&DDRA, &PORTA, &PINA, DCR_BTN_MASK);

	TVOLT moist[3];
	moist[0] = initAdcStruct(234, 2100);
	moist[1] = initAdcStruct(234, 2100);

	systemInit();

	sprintf(firstRowBuffLCD,"INITIALISATION ");
	sprintf(secondRowBuffLCD,"INITIALISATION");
	printSimpleScreen(firstRowBuffLCD, secondRowBuffLCD);
	_delay_ms(500);
	while (1) {
// 		@brief: stops alarm or watering ONLY while it's running
		if(!setTimerFlag) {
			keyLongPress(&selectBtn, NULL, menuIncr);
			if(!isLCD_On()) {
				keyPress(&stopBtn, LCD_PowerUp);
			} else {
				keyPress(&stopBtn, LCD_PowerDown);
			}
		}

		uartCheck(parse_uart_data);
		updateTexts(controlFactor, switchConditions.moistureMin, systemStatus, &currCtrlBuff, &currMoistCtrlBuff, &currSysStatusBuff);

        if(  (SEC_CHANGED_CHECK) ) {
        	SEC_CHANGED_CLEAR;

	        if(MIN_CHANGED_CHECK) {
#if MOIST_SENS_ACTIVE
	        	static uint8_t temporaryLockFlag = 1;
				moisture[sensorNumber] = moistCheckResult(temporaryLockFlag, moist ,&sensorNumber);
	        	temporaryLockFlag = moistCheckStart(sensorNumber);
	        	if(!temporaryLockFlag) {
	        		MIN_CHANGED_CLEAR;
	        	}
#else
	        	MIN_CHANGED_CLEAR;
#endif
#if LOGER_ACTIVE
			writeEEPROM(true);
			loggerStatus = updateLogger(global, _sensVal);
#endif
#if LIGHT_SENS_ACTIVE
	        	voltAdc(adcOversample(0x07, 3), &light); // replace this name with adcConvert(adcOversample(0x07, 3), &light);
		        // TODO: implement pwrUp and pwrDown even though we can check brightness constantly there is no need so better save power
				lightStrength = lightSensor(&light);
#endif
	        }
			temp = readTemp();
			_sensVal = updateSensorValues((moisture[0]+moisture[1])/2, temp.tempMultip, _sensVal.wfVolume, lightStrength);
			switchConditions = updateConditionalSwitch(timeToSeconds(&turnOnTime), switchConditions.moistureMin, wfDoseVol, timerTime);
			systemStatus = conditionalSwitch(switchConditions, _sensVal, timeToSeconds(&global), activateSystem);
			litreTime = (controlFactor == MINUTES)?timerTime:wfDoseVol/1000;
			switchConditions.ctrlFactor = controlFactor;

        	if(resetSecond == 1) {
        		second = 0;
        	}
        	global.second = second;
        	timeDivision(&global);

        	 uint32_t wfTemp = measureWF();
			if(	(!(wfStatus & WF_COUNT_RUNNING)) &&
					wfTemp) {
				_sensVal.wfVolume += wfTemp / 60;	//Converted to absolute mililitres
			}
#if WF_SENS_DEBUG
			wfSensToggle(1);
#else
			wfSensToggle(  ( (systemStatus==WORK)&&(controlFactor==LITRES) )? 1: 0  );
#endif

			if(!setTimerFlag) {
				printMainScreen(turnOnTime, litreTime, global, currMoistCtrlBuff, currCtrlBuff, currSysStatusBuff);
			}

	        //outAlarm();
	        //turnAlarmOff();
#if MAIN_DEBUG_ACTIVE
			uartTxStr("\f");
			uartWriteCurrTime(global);
			uartTxStr("\n\r");
			uartWriteTemp(&temp);
			uartTxStr("\n\r");
			uartWriteLight(lightStrength);
			uartTxStr("\n\r");
			uartWriteMoisture(MOIST_SENSORS_NUMBER, moisture);

			char uartBuff[16];
			uartTxStr("\n\rWf = ");
			sprintf(uartBuff, "%ld", wfTemp);
			uartTxStr(uartBuff);
			uartTxStr(" [ml/min] = ");
			uartTxStr("\n\rwf vol accumulated= ");
			sprintf(uartBuff, "%ld", (uint32_t)_sensVal.wfVolume);
			uartTxStr(uartBuff);
#if LOGER_ACTIVE
			if(loggerStatus == LOGGER_SUCCESS) {
				uartTxStr("\r\nLogger active");
			} else {
				uartTxStr("\r\nLogger error");
			}
#endif
#endif
        }

//	****************** MENU ******************	//
        static uint8_t isBlink = 0;
        static uint8_t inputModified = 0;
		if(inputModified) {
			blinkDelay = BLINK_DELAY_FREQ;
		}
		if(BLINK_DELAY_FREQ == blinkDelay--) {
			printMainScreen(turnOnTime, litreTime, global, currMoistCtrlBuff, currCtrlBuff, currSysStatusBuff);
		} else if(UINT16_MAX == blinkDelay ) {	//It makes one of var to dissapear and appear depend to blink delay set below
			blinkDelay = BLINK_DELAY_FREQ*1.2;	//TODO consider multiplying by 2 (then makes 50/50 blink)
			isBlink = 1;
		}
		static uint32_t lengthON = 0;
		uint32_t temporaryVal;

		switch(setTimerFlag) {

		case 1: //Set system clock
				resetSecond = incrDcr(&incrTimeBtn, &dcrTimeBtn, 0, 0, &global);//TODO: check if reset second working
				inputModified = resetSecond;
				keyPress(&stopBtn, exitServiceMode);
				keyPress(&selectBtn, menuIncr);
				if(isBlink) {
					sprintf(firstRowBuffLCD,"%02d:%02d*%3ld%s*%s", turnOnTime.hour, turnOnTime.minute,
																litreTime, currCtrlBuff, currMoistCtrlBuff);
					sprintf(secondRowBuffLCD,"  :  *%s", currSysStatusBuff);
				}
		break;

		case 2: //Set turn ON time
			inputModified = incrDcr(&incrTimeBtn, &dcrTimeBtn, 0, 0, &turnOnTime);
			keyPress(&stopBtn, exitServiceMode);
			keyPress(&selectBtn, menuIncr);
			if(isBlink) {
				sprintf(firstRowBuffLCD,"  :  *%3ld%s*%s", litreTime, currCtrlBuff, currMoistCtrlBuff);
				sprintf(secondRowBuffLCD,"%02d:%02d*%s", global.hour, global.minute, currSysStatusBuff);
			}
		break;

		case 3:
			keyPress(&stopBtn, exitServiceMode);
			inputModified = keyPress(&incrTimeBtn, toggleCtrl);
			inputModified = keyPress(&dcrTimeBtn, toggleCtrl);
			keyPress(&selectBtn, menuIncr);
			if(isBlink) {
				sprintf(firstRowBuffLCD,"%02d:%02d*%3ld   *%s", turnOnTime.hour, turnOnTime.minute,
																			litreTime, currMoistCtrlBuff);
				sprintf(secondRowBuffLCD,"%02d:%02d*%s", global.hour, global.minute, currSysStatusBuff);
			}
		break;

		case 4: //Set how long to keep ON ( (by time (minutes)||(by volume (litres) )
			inputModified = incrDcr(&incrTimeBtn, &dcrTimeBtn, &lengthON, 999, NULL);
			keyPress(&stopBtn, exitServiceMode);
			keyPress(&selectBtn, menuIncr);
			if(controlFactor == MINUTES) {
				timerTime = lengthON;
				wfDoseVol = 0;
			} else {
				wfDoseVol = (lengthON*1000); //convert [l] into [ml]
				timerTime = 0;
			}
			if(isBlink) {
				sprintf(firstRowBuffLCD,"%02d:%02d*   %s*%s", turnOnTime.hour, turnOnTime.minute,
																		currCtrlBuff, currMoistCtrlBuff);
				sprintf(secondRowBuffLCD,"%02d:%02d*%s", global.hour, global.minute, currSysStatusBuff);
			}
		break;

		case 5:
			//uint32_t tempVara = (uint32_t)switchConditions.moistureMin;
			temporaryVal = (uint32_t)switchConditions.moistureMin;
			inputModified = incrDcr(&incrTimeBtn, &dcrTimeBtn, &temporaryVal, 99, NULL);
			keyLongPress(&stopBtn, NULL, exitServiceMode);	//TODO: change workStatus performed by function
			switchConditions.moistureMin = temporaryVal;
			if((switchConditions.moistureMin) && (moistCtrl != MOIST_CHECK_ON)) {
				moistCtrl = MOIST_CHECK_ON;
			} else if((!switchConditions.moistureMin) && (moistCtrl != MOIST_CHECK_OFF)) {
				moistCtrl = MOIST_CHECK_OFF;
			}

			keyPress(&selectBtn, menuIncr);
			if(isBlink) {
				sprintf(firstRowBuffLCD,"%02d:%02d*%3ld%s*   ", turnOnTime.hour, turnOnTime.minute, litreTime, currCtrlBuff);
				sprintf(secondRowBuffLCD,"%02d:%02d*%s", global.hour, global.minute, currSysStatusBuff);
			}
		break;

		case 6:
			registerStartActionCallback(relON);
			registerEndActionCallback(relOFF);
			menuIncr();
		break;

		case 7:
			inputModified = keyPress(&incrTimeBtn, activDeactivSystem);
			inputModified = keyPress(&dcrTimeBtn, activDeactivSystem);
			keyPress(&selectBtn, menuIncr);
			if(isBlink) {
				sprintf(firstRowBuffLCD,"%02d:%02d*%3ld%s*%s", turnOnTime.hour, turnOnTime.minute,
															litreTime, currCtrlBuff, currMoistCtrlBuff);
				sprintf(secondRowBuffLCD,"%02d:%02d*", global.hour, global.minute);
			}
		break;

		case 8:
			setTimerFlag = 0;
		break;

		default:
			if(setTimerFlag) {
				setTimerFlag = 0;
			}
			inputModified = 0;
		}

		if(isBlink) {
			printSimpleScreen(firstRowBuffLCD, secondRowBuffLCD);
			isBlink = 0;
		}
#if 0
		goToSleep();
#endif
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

//	powerSaveInit();
	rt_clock_init();
	LCD_Initalize();
	adcInit();

	uartInit( UART_BAUD_SELECT(115200, 16000000L) );
	uartFlush();
#if LOGER_ACTIVE
	logerInit();
#endif
	MIN_CHANGED_SET;
	turnAlarmOff();

	sensorNumber  = 0;
	wfStatus = 0;
}

/*************************************************************************
 Function: 	printMainScreen()
 Purpose:	Prints basic screen
 Input	:	timer - how long to wait from start tilll the end of workStatus
 	 	 	turnOnClock	-
 	 	 	globalClock
 **************************************************************************/
void printMainScreen(time turnOnClock, uint32_t ctrlVar, time globalClock, char* sysMoistStat,  char*  currCtrlBuff, char* sysStat) {
	sprintf(firstRowBuffLCD,"%02d:%02d*%3ld%s*%s", turnOnClock.hour, turnOnClock.minute,
													ctrlVar, currCtrlBuff, sysMoistStat);
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

void activDeactivSystem(void) {
	if(activateSystem == 0) {
		activateSystem = 1;
	} else {
		activateSystem = 0;
	}
}

TEMP readTemp(void) {
	static uint8_t conversionDone = 1;
	static TEMP tmpTemp;
	if(!conversionDone) {			//	Start conversion
		ds18b20_ConvertT();
		conversionDone = 1;
	} else {				// Read converted temperature
		readTemperature(&tmpTemp);
		conversionDone = 0;
	}
	return tmpTemp;
}

value initSensVal() {
	value tempSensVal;
	tempSensVal.bright= 0;
	tempSensVal.moisture= 0;
	tempSensVal.temp= 0;
	tempSensVal.wfVolume = 0;
	return tempSensVal;
}

condSwitch initCondSwitch() {
	condSwitch tempCondSwitch;
	tempCondSwitch.moistCtrl= MOIST_CONTROL_DEFAULT;
	tempCondSwitch.ctrlFactor= CONTROL_FACTOR_DEFAULT;
	tempCondSwitch.moistureMin = 0;
	tempCondSwitch.presetWf= 0;
	tempCondSwitch.procesTime = 0;
	tempCondSwitch.turnOnTime = 0;
	return tempCondSwitch;
}
