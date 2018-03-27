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
const char systemReady[] = "GOTOWY";
const char systemWorking[] = "PRACA";
const char* litreCtrlTxt = "ltr";
const char* minuteCtrlTxt = "min";
char* currControl;

int main(void) {
	uint8_t minMoist = 0;
	uint32_t wfDoseVol = 0;
	uint8_t temporaryLockFlag = 1;

	uint32_t wfVolAccu = 0;
	uint32_t wfTemp = 0;

	uint8_t resetSecond = 0;

//TODO: implement macro defining button port letter (eg. "A", or "B")
	tButton clockBtn = btnInit(&DDRD, &PORTD, &PIND, SELECT_BTN_MASK);
	tButton turnOnTimeBtn = btnInit(&DDRD, &PORTD, &PIND, STOP_BTN_MASK);
	tButton timerBtn = btnInit(&DDRD, &PORTD, &PIND, CTRL_BTN_MASK);
	tButton incrTimeBtn = btnInit(&DDRA, &PORTA, &PINA, INCR_BTN_MASK);
	tButton dcrTimeBtn = btnInit(&DDRA, &PORTA, &PINA, DCR_BTN_MASK);
	tButton volumeBtn = btnInit(&DDRD, &PORTD, &PIND, VOL_BTN_MASK);
	tButton ctrlModeBtn = btnInit(&DDRD, &PORTD, &PIND, SET_BTN_MASK); //TODO: implement this button

//	tButton humidityBtn = btnInit(&DDRD, &PORTD, &PIND, HUMIDITY_BTN_MASK);

	TVOLT moist[3];
	moist[0].ref_v = 234;
	moist[0].ref_adc = 2100;

	moist[1].ref_v = 234;
	moist[1].ref_adc = 2100;

	blinkDelay = BLINK_DELAY_FREQ;
	controlFactor = LITRES;
	systemStatus = READY;

	systemInit();

	while (1) {
// 		@brief: stops alarm or watering ONLY while it's running
//		keyPress(&turnOnTimeBtn, 0);
		if(!setTimerFlag) {
			keyLongPress(&clockBtn, NULL, setClockMode);
			keyLongPress(&turnOnTimeBtn, NULL, setTimeOnMode);
			keyLongPress(&timerBtn, NULL, setTimerMode);
			keyLongPress(&volumeBtn, NULL, setVolumeMode);
			keyLongPress(&ctrlModeBtn, NULL, setCtrlMode);
		}
#if 0
 		keyLongPress(&volumeBtn, setVolumeMode, serviceModeEntry);
		keyLongPress(&ctrlModeBtn, setCtrlMode, serviceModeEntry);
		keyLongPress(&humidityBtn, setHumidityMode, serviceModeEntry);
#endif

		uartCheck(parse_uart_data);

		if((controlFactor==MINUTES) && (currControl!=&minuteCtrlTxt[0])) {
			currControl= minuteCtrlTxt;
		} else if(currControl!=&litreCtrlTxt[0]) {
			currControl= litreCtrlTxt;
		}

        if((SEC_CHANGED_CHECK) && (!setTimerFlag)) {
        	SEC_CHANGED_CLEAR;

	        if(MIN_CHANGED_CHECK) {
				// TODO take the lowest or avarage moisture and pass it timerWithMoist (which doesn't exist)
				moisture[sensorNumber] = moistCheckResult(temporaryLockFlag, moist ,&sensorNumber);
	        	temporaryLockFlag = moistCheckStart(sensorNumber);
	        	if(!temporaryLockFlag) {
	        		MIN_CHANGED_CLEAR;
	        	}
	        }

        	if(resetSecond == 1) {
        		second = 0;
        	}
        	global.second = second;
        	timeDivision(&global);
        	uint8_t tmp = timerSwitch(timeToSeconds(&global), timeToSeconds(&turnOnTime), timeToSeconds(&activeTime) );
			if(tmp) {
				systemStatus = WORK;
			} else {
				systemStatus = READY;
			}

			if(	(!(wfStatus & WF_COUNT_RUNNING)) &&
					wfTemp) {
				wfVolAccu += wfTemp / 60;
			}
			wfSensToggle(  ( (systemStatus==WORK)&&(controlFactor==LITRES) )? 1: 0  );

			if(!setTimerFlag) {
				printMainScreen();
			}
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

		if((BLINK_DELAY_FREQ/2) == blinkDelay--) {
			printMainScreen();
		} else if((BLINK_DELAY_FREQ/2) > blinkDelay) {

		} else if(BLINK_DELAY_FREQ < blinkDelay) {
			blinkDelay = BLINK_DELAY_FREQ;

			switch(setTimerFlag) { //GUI switch delayed due to screen refresh

			case 1: //Set system clock
				sprintf(firstRowBuffLCD,"%03dmin*%02d:%02d*NIE", activeTime.minute, turnOnTime.hour, turnOnTime.minute);
				sprintf(secondRowBuffLCD,"  :  *%s", (systemStatus==READY) ? systemReady : systemWorking);
			break;

			case 2: //Set turn ON time
				sprintf(firstRowBuffLCD,"%03dmin*  :  *NIE", activeTime.minute);
				sprintf(secondRowBuffLCD,"%02d:%02d*%s", global.hour, global.minute,
										(systemStatus==READY) ? systemReady : systemWorking);
			break;

			case 3: //Set how long to keep on (in case control by time (minutes))
				sprintf(firstRowBuffLCD,"   %s*%02d:%02d*NIE", currControl, turnOnTime.hour, turnOnTime.minute);
				sprintf(secondRowBuffLCD,"%02d:%02d*%s", global.hour, global.minute,
										(systemStatus==READY) ? systemReady : systemWorking);
			break;

			case 4: //Set how long to keep on (in case control by volume (litres))
				sprintf(firstRowBuffLCD,"   %s* %02d:%02d*NIE", currControl,turnOnTime.hour,  turnOnTime.minute);
				sprintf(secondRowBuffLCD,"%02d:%02d*%s", global.hour, global.minute,
										(systemStatus==READY) ? systemReady : systemWorking);
			break;

			case 5: //Set how long to keep on (in case control by volume (litres))
				sprintf(firstRowBuffLCD,"%03d   *%02d:%02d*NIE", activeTime.minute, turnOnTime.hour, turnOnTime.minute);
				sprintf(secondRowBuffLCD,"%02d:%02d*%s", global.hour, global.minute,
										(systemStatus==READY) ? systemReady : systemWorking);
			break;

			default:
				printMainScreen();
			}
			printSimpleScreen(firstRowBuffLCD, secondRowBuffLCD);
		}


		switch(setTimerFlag) { //System switch without delay

		case 1: //Set system clock
			//TODO: check if reset second working
			incrDcr(&incrTimeBtn, &dcrTimeBtn, 0, 0, &global);
			keyPress(&clockBtn, exitServiceMode);
		break;

		case 2: //Set turn ON time
			incrDcr(&incrTimeBtn, &dcrTimeBtn, 0, 0, &turnOnTime);
			keyPress(&turnOnTimeBtn, exitServiceMode);
		break;

		case 3: //Set how long to keep ON (by time (minutes))
			incrDcr(&incrTimeBtn, &dcrTimeBtn, 0, 0, &activeTime);
			keyPress(&timerBtn, exitServiceMode);
		break;

		case 4: //Set how long to keep on (by volume (litres))
		//	incrDcr(&incrTimeBtn, &dcrTimeBtn, &wfDoseVol, 9999, 0);	//Maximum 9999 liters as decilitre
			//TODO:add exit button
			keyLongPress(&volumeBtn, NULL, setVolumeMode);
		break;

		case 5:
			keyLongPress(&ctrlModeBtn, NULL, exitServiceMode);
			keyLongPress(&incrTimeBtn, NULL, controlMode);
			keyLongPress(&dcrTimeBtn, NULL, controlMode);
		break;

		case 6:
			registerStartActionCallback(relON);
			registerEndActionCallback(relOFF);
			setTimerFlag = 0;
		break;

		case 7:
#if 0
		if(calibrateMoistSensor(1, setTimerFlag, moisture[0], &clockBtn, &turnOnTimeBtn) == 0)	{
			continue;		//Prevent from entering classic MENU
		}
#endif
		break;

		default:
			if(setTimerFlag) {
				setTimerFlag = 0;
			}
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
	//TODO: remove dsFlag, lightStrength
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
 **************************************************************************/
void printMainScreen(void) {
	sprintf(firstRowBuffLCD,"%03d%s*%02d:%02d*NIE", activeTime.minute, currControl, turnOnTime.hour, turnOnTime.minute);
	sprintf(secondRowBuffLCD,"%02d:%02d*%s", global.hour, global.minute,
											(systemStatus==READY) ? systemReady : systemWorking);
	printSimpleScreen(firstRowBuffLCD, secondRowBuffLCD);
}

/*************************************************************************
 Function: 	printMainScreen()
 Purpose:	Prints basic screen
 **************************************************************************/
void controlMode(void) {
	if(controlFactor == LITRES) {
		controlFactor = MINUTES;
	} else {
		controlFactor = LITRES;
	}
}
