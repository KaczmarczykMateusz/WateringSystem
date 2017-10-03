/*
 ============================================================================
 Name        : main.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "main.h"
#include "math.h"

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

	uint8_t dsFlag = 0;
	uint8_t moistCheck = 1;

	TVOLT light;
	light.ref_v = 234;
	light.ref_adc = 2100;

	TVOLT moist[3];
	moist[0].ref_v = 234;
	moist[0].ref_adc = 2100;

	moist[1].ref_v = 234;
	moist[1].ref_adc = 2100;

	uartInit( UART_BAUD_SELECT(9600, 16000000L) );
	uartFlush();
	relOFF();
	outOFF();
	alarmActive = 0;
	uint8_t resetSecond;

	while (1) {
		if(setTime(&setBtn, &global)) {
			resetSecond = 1;
		} else {
			resetSecond = 0;
		}

		ds18b20_ConvertT();
		keyLongPress(&selectBtn, relON, timerSetMode);
		keyLongPress(&stopBtn, turnAlarmOff, NULL);

		uartCheck(parse_uart_data);

        if(SEC_CHANGED_CHECK) {
	        if(MIN_CHANGED_CHECK) {
	        	moistCheck = 1;
	        	userTimer(timeToSeconds(&turnOnTime), timeToSeconds(&turnOffTime), turnAlarmOn, turnAlarmOff, timeToSeconds(&global));
	        	MIN_CHANGED_CLEAR;
	        }

	        moistCheckStart(&moistCheck, &sensorNumber);

           	uint16_t adcOversampled = adcOversample(0x07, 3);
            voltAdc(adcOversampled, &light);
            lightStrength = lightSensor(&light);

        	if(resetSecond) {
        		second = 0;
        	}
        	global.second = second;
        	timeDivision(&global);

			// TODO: Imlement UART error check

			sprintf(printLCDBuffer,"%02d:%02d:%02d %d%%",global.hour, global.minute, global.second, lightStrength );
			LCD_Clear();
            LCD_GoTo(0,0);					
			LCD_WriteText(printLCDBuffer);
			SEC_CHANGED_CLEAR;

			if(dsFlag == 1) {
				readTemperature(&temp);
				sprintf(currTemp,"%2ld.%01ld\xdf""C", temp.tempInt, temp.tempFract);
				dsFlag = 0;
			} else {
				dsFlag = 1;
			}
			moistCheckResult(moistCheck, moist ,&sensorNumber, moisture);
			uartPuts("\f");
			uartWriteCurrTime();
			uartPuts("\n\r");
			uartWriteTemp(&temp);
			uartPuts("\n\r");
			uartWriteLight(lightStrength);
			uartPuts("\n\r");
			uartWriteMoisture(MOIST_SENSORS_NUMBER, moisture);
			LCD_GoTo(0,1);
			LCD_WriteText(currTemp);
	        outAlarm();

			while(setTimerFlag) {
				keyPress(&selectBtn, exitTimerSetOnMode);
				setTime(&setBtn, &turnOnTime);
				sprintf(printLCDBuffer,"%02d:%02d:%02d", turnOnTime.hour, turnOnTime.minute, turnOnTime.second);
				if(SEC_CHANGED_CHECK) {
					LCD_Clear();
					LCD_WriteText("Set timer ON");
					LCD_GoTo(0,1);
					LCD_WriteText(printLCDBuffer);
					SEC_CHANGED_CLEAR;
					if(!executedFlag) {
						executedFlag = 1;
					}
		        	global.second = second;
					timeDivision(&global);
				}
			}
			while(executedFlag) {
				setTime(&setBtn, &turnOffTime);
				keyLongPress(&selectBtn, exitTimerSetMode, NULL);
				sprintf(printLCDBuffer,"%02d:%02d:%02d", turnOffTime.hour, turnOffTime.minute, turnOffTime.second);
				if(SEC_CHANGED_CHECK) {
					LCD_Clear();
					LCD_WriteText("Set timer OFF");
					LCD_GoTo(0,1);
					LCD_WriteText(printLCDBuffer);
					SEC_CHANGED_CLEAR;
		        	global.second = second;
					timeDivision(&global);
				}
			}
        }
//      goToSleep();
	}
}
