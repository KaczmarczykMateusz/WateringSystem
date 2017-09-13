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
    keyboardInit();
	rt_clock_init();
	LCD_Initalize();
	adcInit();

    tButton setBtn;
    tButton selectBtn;
	setBtn = setBtnInit();
	selectBtn = selectBtnInit();

	time global;
//	time user;

	TVOLT light;
	light.ref_v = 234;
	light.ref_adc = 2100;

	TVOLT moist;
	moist.ref_v = 234;
	moist.ref_adc = 2100;
	uint8_t temporary;
	uart_init( UART_BAUD_SELECT(9600, 16000000L) );
	uart_flush();
	int16_t temporar;

	char *message = "egg";

	while (1) {

		setTime(&setBtn, &global);
/*
		key_press(&selectBtn, relOFF, relOFF);
		key_push_up(&selectBtn, relON, relON);
*/
//		key_push_up(&selectBtn, aa=getCurrentTime(&user), NULL); // not working

		//sending byte
/*
		if(uart_available != 0) {
			uart_putc(23);
			uart_putc('jajko');
		} else {
			LCD_Clear();
			LCD_GoTo(0,0);
			LCD_WriteText("Tx buffer full");
		}
*/
		if(uart_peek() != UART_NO_DATA) {
			temporar = uart_getc();
			temporar &= ~(0xFF00);
			//temporar = (temporar >> 8); // TODO: Imlement error check
			sprintf(printLCDBuffer,"%d", temporar);
			LCD_Clear();
			LCD_GoTo(0,0);
			LCD_WriteText(printLCDBuffer);
			_delay_ms(500);
		}

        if(timeChanged == 1) {

//			uint16_t received = uart_getc();
//			uint8_t uartRxError = (received >> 8);
//			received = received & ~(0xF0);

        	//uart_puts("eggg");

/*
 *         	key_push_up(&selectBtn, getCurrentTime(&user), NULL); // not working
			getCurrentTime(&global);
			if(saveTime != getCurrentTime());
*/
           	uint16_t adcOversampled = adcOversample(0x07, 3);
            voltAdc(adcOversampled, &moist);
            voltAdc(adcOversampled, &light);

        	temporary = lightSensor(&light);
        	global.second = second;
        	timeDivision(&global);


			sprintf(printLCDBuffer,"%02d:%02d:%02d %d",global.hour, global.minute, global.second, temporary );
			LCD_Clear();
            LCD_GoTo(0,0);					
			LCD_WriteText(printLCDBuffer);
			timeChanged = 0;

    		sprintf(printLCDBuffer,"%s.%s", moist.beforeComa, moist.afterComa );
    		LCD_GoTo(0,1);
    		LCD_WriteText(printLCDBuffer);
/*
    		sprintf(printLCDBuffer,"%s.%sV", light.beforeComa, light.beforeComa);
    		LCD_GoTo(0,1);
    		LCD_WriteText(printLCDBuffer);
    		_delay_ms(1000);
*/
        }

  //      goToSleep();
	}
}
/*
			//LCD_WriteText(printLCDBuffer);		// send counter value to display
		    ds18b20_Read(ds18b20_pad);

			temp = ((ds18b20_pad[1] << 8) + ds18b20_pad[0]) / 16.0 ;

			// Formuuje komunikat w tablicy 'str' 
			sprintf(str,"%4.1f\xdf""C", temp);
			sprintf(str, "%d",ds18b20_pad);
			LCD_GoTo(0,1);
			LCD_WriteText(str);
*/

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
