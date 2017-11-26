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

int main(void) {
	uint8_t minMoist = 0;
	uint8_t dsFlag = 0;
	uint8_t temporaryLockFlag = 1;

	uint32_t wfVolAccu = 0;
	uint32_t wfTemp = 0;
	uint32_t wfDoseVol = 0;

	uint8_t resetSecond = 0;
 	uint32_t passAsLong = 0;
//TODO: implement macro defining button port letter (eg. "A", or "B")
	tButton setBtn = btnInit(&DDRD, &PORTD, &PIND, SET_BTN_MASK);
	tButton selectBtn = btnInit(&DDRD, &PORTD, &PIND, SELECT_BTN_MASK);
	tButton stopBtn = btnInit(&DDRD, &PORTD, &PIND, STOP_BTN_MASK);

	TVOLT light;
	light.ref_v = 234;
	light.ref_adc = 2100;

	TVOLT moist[3];
	moist[0].ref_v = 234;
	moist[0].ref_adc = 2100;

	moist[1].ref_v = 234;
	moist[1].ref_adc = 2100;

	condSwitch switchConditions; // todo: throw it out from header file and define here
	value val;

	systemInit();

	while (1) {
		if(!lockMainScreen) {
			resetSecond = incrDcr(&setBtn, &stopBtn, 0, 0, &global) ? 1 : 0;
			// @brief: stops alarm or watering ONLY while it's running
	//		keyPress(&stopBtn, 0);
		}

		keyLongPress(&selectBtn, timerSetModeNextStep, serviceModeEntry);
#if 0
		if(calibrateMoistSensor(enbleSrvceMode, setTimerFlag, moisture[0], &setBtn, &stopBtn) == 0)	{
			continue;		//Prevent from entering classic MENU
		}
#endif
		uartCheck(parse_uart_data);

        if(	(SEC_CHANGED_CHECK) &&
        	(!lockMainScreen)) {
        	SEC_CHANGED_CLEAR;

	        if(MIN_CHANGED_CHECK) {
	        	voltAdc(adcOversample(0x07, 3), &light); // replace this name with adcConvert(adcOversample(0x07, 3), &light);

// TODO: implement pwrUp and pwrDown even though we can check brightness constantly there is no need so better save power
	        	lightStrength = lightSensor(&light);

//TODO: remove it from current project for further use in other
//	        	userTimer(timeToSeconds(&turnOnTime), timeToSeconds(&activeTime), turnAlarmOn, activeTime, timeToSeconds(&global)); // calls simple alarm

	        	//Keep minute flag ON until moisture sensors update
//TODO: add INPUT_WAIT_MOIST
	        	if(!moistCheckStart(&temporaryLockFlag, &sensorNumber)) {
	        		MIN_CHANGED_CLEAR;
	        	}
	        }

        	if(resetSecond == 1) {
        		second = 0;
        	}
        	global.second = second;
        	timeDivision(&global);

			// TODO: Imlement UART error check
        	// TODO: Imlement temperature sensor error display

        	// @brief: check temperature,letting sensor at least 750ms for conversion (in this case even more than one second)
			if(!dsFlag) {			//	Start conversion
				ds18b20_ConvertT();
				dsFlag = 1;
			} else {				// Read converted temperature
				readTemperature(&temp);
				sprintf(currTemp, "%2ld.%01ld\xdf""C", temp.tempInt, temp.tempFract);
				dsFlag = 0;
			}

			uartPuts("\f");
			uartWriteCurrTime();
			uartPuts("\n\r");
			uartWriteTemp(&temp);
			uartPuts("\n\r");
			uartWriteLight(lightStrength);
			uartPuts("\n\r");
			uartWriteMoisture(MOIST_SENSORS_NUMBER, moisture);

			//Waterflow measurnment block TODO: optimize code below and linked t it
			char abuf[16];
			uartPuts("\n\rwf vol accumulated= ");
			sprintf(abuf, "%ld", wfVolAccu); //TODO, check if this solves issue of too short result
			uartPuts(abuf);
			uartPuts("\n\rMeasureWF() = ");
			wfTemp = measureWF();
			sprintf(abuf, "%ld", wfTemp); //TODO, check if this solves issue of too short result
			uartPuts(abuf);

			if(	(!(wfStatus & WF_COUNT_RUNNING)) &&
					wfTemp) {
				wfVolAccu += wfTemp / 60;
			}

			wfSensToggle(1);

			if(!lockMainScreen) {
				sprintf(printLCDBuffer,"%02d:%02d:%02d %d%%",global.hour, global.minute, global.second, lightStrength );
				LCD_Clear();
				LCD_GoTo(0,0);
				LCD_WriteText(printLCDBuffer);
				LCD_GoTo(0,1);
				LCD_WriteText(currTemp);
			}

	        outAlarm();
	        //turnAlarmOff();
// TODO take the lowest or avarage moisture and pass it to conditional switch
// TODO: Pass wfDoseVol as [dl]
			wfDoseVol *= 10; // Convert [dl] to [cl];
	        updateConditionalSwitch(&switchConditions, timeToSeconds(&turnOnTime), timeToSeconds(&activeTime), minMoist, wfDoseVol);
	        //	  updateConditionalSwitch(&switchConditions, 60, 120, 10, 100);
			wfDoseVol /= 10; // Convert back [cl] to [dl];
			updateSensorValues(&val, moisture[0], temp.tempMultip, wfVolAccu, lightStrength);
			conditionalSwitch(&switchConditions, &val, timeToSeconds(&global), &alarmActive);// TODO:implement enabling/ disabling alarm

			moisture[sensorNumber] = moistCheckResult(temporaryLockFlag, moist ,&sensorNumber);
        }


//	****************** MENU ******************	//
		switch(setTimerFlag) {

		case 1:
			incrDcr(&setBtn, &stopBtn, 0, 0, &turnOnTime);
			sprintf(printLCDBuffer, "Set timer ON");
			sprintf(buff1, "%02d:%02d:%02d", turnOnTime.hour, turnOnTime.minute, turnOnTime.second);
			menuItem(printLCDBuffer, buff1);
		break;

		case 2:
			incrDcr(&setBtn, &stopBtn, 0, 0, &activeTime);
			sprintf(printLCDBuffer, "Set period");
			sprintf(buff1, "%02d:%02d:%02d", activeTime.hour, activeTime.minute, activeTime.second);
			menuItem(printLCDBuffer, buff1);
		break;

		case 3:
			passAsLong = (uint32_t)minMoist;
			incrDcr(&setBtn, &stopBtn, &passAsLong, 100, 0);
			minMoist = (uint8_t)passAsLong;
			sprintf(printLCDBuffer, "Minimum moisture");
			sprintf(buff1, "%d%%", minMoist);
			menuItem(printLCDBuffer, buff1);
		break;

		case 4:
			incrDcr(&setBtn, &stopBtn, &wfDoseVol, 10000, 0);	//Maximum 1000 liters as decilitre
			sprintf(printLCDBuffer, "Water dose vol.");
// TODO: Display wfDoseVol as [dl]
			sprintf(buff1, "%ld [dl]", wfDoseVol);
			menuItem(printLCDBuffer, buff1);
			registerStartActionCallback(turnAlarmOn);
			registerEndActionCallback(turnAlarmOff);
			//registerStartActionCallback(relON);
			//registerStartActionCallback(relOFF);

		break;

		case 5:
			setTimerFlag = 0;
			lockMainScreen = 0;
		break;

		default:
			setTimerFlag = 0;
			lockMainScreen = 0;
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
	enbleSrvceMode = 0;
	ALARM_INIT;
	RELAY_INIT;

	ALARM_OFF;
	relOFF();
	outOFF();
//	RELAY_ON;

//	powerSaveInit();
	rt_clock_init();
	LCD_Initalize();
	adcInit();

	uartInit( UART_BAUD_SELECT(115200, 16000000L) );
	uartFlush();

	MIN_CHANGED_SET;
	lockMainScreen = 0;
	turnAlarmOff();

	sensorNumber  = 0;
	wfStatus = 0;
}
