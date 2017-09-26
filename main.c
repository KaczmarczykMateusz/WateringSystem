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
    OUTPUT_PIN_A111_ON;

//	powerSaveInit();
	rt_clock_init();
	LCD_Initalize();
	adcInit();

	tButton setBtn = btnInit(&DDRD, &PORTD, &PIND, SET_BTN_MASK);
	tButton selectBtn = btnInit(&DDRD, &PORTD, &PIND, SELECT_BTN_MASK);

	uint16_t sendDigit = 84;
	uint8_t dsFlag;
	time global;
	time turnOnTime;
	time turnOffTime;

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

	while (1) {
		setTime(&setBtn, &global);

		ds18b20_ConvertT();
		keyLongPress(&selectBtn, relON, timerSetMode);
		uartCheck(parse_uart_data);

        if(timeChanged == 1) {
           	uint16_t adcOversampled = adcOversample(0x07, 3);
            voltAdc(adcOversampled, &moist);
            voltAdc(adcOversampled, &light);

        	temporary = lightSensor(&light);
        	global.second = second;
        	timeDivision(&global);

        	if(sendDigit < 999) {
				sendDigit += 2;
        	} else {
        		sendDigit = 0;
        	}
        	//uartPuts("Second passed    ");
        	sendInteger(sendDigit);

/*
			if(uartPeek() != UART_NO_DATA) {
				temporar = uartGetc();
				temporar &= ~(0xFF00);
				//temporar = (temporar >> 8); // TODO: Imlement error check
				sprintf(printLCDBuffer,"%c", temporar);
				LCD_Clear();
				LCD_GoTo(0,0);
				LCD_WriteText(printLCDBuffer);
			}
*/

			sprintf(printLCDBuffer,"%02d:%02d:%02d %d",global.hour, global.minute, global.second, temporary );
			LCD_Clear();
            LCD_GoTo(0,0);					
			LCD_WriteText(printLCDBuffer);
			timeChanged = 0;
			if(dsFlag == 1) {
			ds18b20_Read(ds18b20_pad);
			temp = ((ds18b20_pad[1] << 8) + ds18b20_pad[0]) / 16.0 ;
			// Formuuje komunikat w tablicy 'str'
			sprintf(str,"%4.1f\xdf""C", temp);

			dsFlag = 0;
			} else {
				dsFlag = 1;
			}
			LCD_GoTo(0,1);
			LCD_WriteText(str);
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
				keyPress(&selectBtn, exitTimerSetMode);
				setTime(&setBtn, &turnOnTime);
				sprintf(printLCDBuffer,"%02d:%02d:%02d", turnOnTime.hour, turnOnTime.minute, turnOnTime.second);
				LCD_Clear();
				LCD_WriteText("Set timer ON");
				LCD_GoTo(0,1);
				LCD_WriteText(printLCDBuffer);
			}
		while(setTimerFlag) {
			setTime(&setBtn, &turnOffTime);
			keyPress(&selectBtn, exitTimerSetMode);
			sprintf(printLCDBuffer,"%02d:%02d:%02d", turnOffTime.hour, turnOffTime.minute, turnOffTime.second);
			LCD_Clear();
			LCD_WriteText("Set timer OFF");
			LCD_GoTo(0,1);
			LCD_WriteText(printLCDBuffer);
		}
//      goToSleep();
	}
}
void timerSetMode(void) {
	setTimerFlag = 1;
}

void exitTimerSetMode(void) {
	setTimerFlag = 0;
}

void relOFF(void) {
	RELAY_OFF;
}

void relON(void) {
	RELAY_ON;
}

void outOFF(void) {
	OUTPUT_PIN_A111_OFF;
}

void outON(void) {
	OUTPUT_PIN_A111_ON;
}
