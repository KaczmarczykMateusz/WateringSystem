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


	TVOLT light;
	light.ref_v = 234;
	light.ref_adc = 2100;

	TVOLT moist;
	moist.ref_v = 234;
	moist.ref_adc = 2100;
	uint8_t temporary;
	uartInit( UART_BAUD_SELECT(9600, 16000000L) );
	uartFlush();
	uint8_t uartTrial;
	uartTrial = 0;
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
           	uint16_t adcOversampled = adcOversample(0x07, 3);
            voltAdc(adcOversampled, &moist);
            voltAdc(adcOversampled, &light);

        	temporary = lightSensor(&light);
        	if(resetSecond) {
        		second = 0;
        	}
        	global.second = second;
        	timeDivision(&global);

			// TODO: Imlement UART error check


			sprintf(printLCDBuffer,"%02d:%02d:%02d %d%%",global.hour, global.minute, global.second, temporary );
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

			uartPuts("\f");
			uartWriteCurrTime();
			uartPuts("\n\r");
			uartWriteTemp(&temp);

			LCD_GoTo(0,1);
			LCD_WriteText(currTemp);
	        outAlarm();


	        if(MIN_CHANGED_CHECK) {
	        	userTimer(timeToSeconds(&turnOnTime), timeToSeconds(&turnOffTime), turnAlarmOn, turnAlarmOff, timeToSeconds(&global));
	        	MIN_CHANGED_CLEAR;
	        }
/*
    		sprintf(printLCDBuffer,"%s.%s", moist.beforeComa, moist.afterComa );
    		LCD_GoTo(0,1);
    		LCD_WriteText(printLCDBuffer);

    		sprintf(printLCDBuffer,"%s.%sV", light.beforeComa, light.beforeComa);
    		LCD_GoTo(0,1);
    		LCD_WriteText(printLCDBuffer);
    		_delay_ms(1000);
*/
        }


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
//      goToSleep();
	}
}




