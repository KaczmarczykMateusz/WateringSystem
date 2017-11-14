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

int main(void)
{
	OUTPUT_PIN_A111_INIT;
	RELAY_INIT;
	ALARM_ON;

//	powerSaveInit();
	rt_clock_init();
	LCD_Initalize();
	adcInit();

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

	uartInit( UART_BAUD_SELECT(115200, 16000000L) );
	uartFlush();
	relOFF();
	outOFF();

	registerStartActionCallback(turnAlarmOn);
	registerEndActionCallback(turnAlarmOff);

	uint8_t dsFlag = 0;
	MIN_CHANGED_SET;
	lockMainScreen = 0;
	alarmActive = 0;
	uint8_t resetSecond = 0;
	minMoist = 0;
	sensorNumber  = 0;

	uint8_t temporaryLockFlag = 1;
	while (1) {
		if(!lockMainScreen) {
			resetSecond = incrDcr(&setBtn, NULL, NULL, &global) ? 1 : 0;
			keyPress(&stopBtn, turnAlarmOff);	// @brief: stops alarm or watering ONLY while it's running
		}

		keyLongPress(&selectBtn, timerSetModeNextStep, relON);

		uartCheck(parse_uart_data);

        if(	(SEC_CHANGED_CHECK) &&
        	(!lockMainScreen)) {
        	SEC_CHANGED_CLEAR;

	        if(MIN_CHANGED_CHECK) {
	        	voltAdc(adcOversample(0x07, 3), &light); // replace this name with adcConvert(adcOversample(0x07, 3), &light);
	        	lightStrength = lightSensor(&light); // TODO: implement pwrUp and pwrDown even though we can check brightness constantly there is no need so better save power

	//        	userTimer(timeToSeconds(&turnOnTime), timeToSeconds(&activeTime), turnAlarmOn, activeTime, timeToSeconds(&global)); // calls simple alarm
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

			if(!lockMainScreen) {
				sprintf(printLCDBuffer,"%02d:%02d:%02d %d%%",global.hour, global.minute, global.second, lightStrength );
				LCD_Clear();
				LCD_GoTo(0,0);
				LCD_WriteText(printLCDBuffer);
				LCD_GoTo(0,1);
				LCD_WriteText(currTemp);
			}
	        outAlarm();
// TODO take the lowest or avarage moisture and pass it to conditional switch
			updateConditionalSwitch(timeToSeconds(&turnOnTime), timeToSeconds(&activeTime), minMoist, 0);
			updateSensorValues(moisture[0], temp.tempMultip, 0, lightStrength);
			conditionalSwitch(timeToSeconds(&global), alarmActive);// TODO:implement enabling/ disabling alarm

 //       	if(!temporaryLockFlag) {
        		moisture[sensorNumber] = moistCheckResult(temporaryLockFlag, moist ,&sensorNumber);
   //     		temporaryLockFlag = 1;
    //    	}
        }


//	****** MENU *******
		switch(setTimerFlag) {

		case 1:
			incrDcr(&setBtn, &stopBtn, NULL, &turnOnTime);
			sprintf(printLCDBuffer, "Set timer ON");
			menuItem(&setTimerFlag, &turnOnTime, &setBtn, printLCDBuffer);
		break;

		case 2:
			incrDcr(&setBtn, &stopBtn, NULL, &activeTime);
			sprintf(printLCDBuffer, "Set period");
			menuItem(&setTimerFlag, &activeTime, &setBtn, printLCDBuffer);
		break;

		case 3:
			incrDcr(&setBtn, &stopBtn, &minMoist, NULL);
			sprintf(printLCDBuffer, "Minimum moisture");
			menuItem(&minMoist, NULL, &setBtn, printLCDBuffer);
		break;

		case 4:
			setTimerFlag = 0;
			lockMainScreen = 0;
		break;

		default:
			setTimerFlag = 0;
		}
//      goToSleep();
	}
}
