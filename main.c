// CPU 16MHz  
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>  
#include <avr/io.h>
#include "main.h"
#include "lcd.h"
#include "ds18b20.h"
#include "clock.h"
#include "WaterFlowSensor.h"
#include "keyboard.h"

/*
#include "keyboard.c"
#include "clock.c"
#include "ds18b20.c"
#include "WaterFlowSensor.c"

*/


int main(void)
{
	clockInit();	
	LCD_Ini();

	TCCR1A= 0x00;
 
    wf = 0;
	
	OUTPUT_PIN_A111_INIT;
	RELAY_INIT;
	
    RELAY_OFF;
    OUTPUT_PIN_A111_ON;


    _delay_ms(500);
    keyboardInit();

	while (1) {
		setTimeF(&PIN_SET_BTN, SET_BTN);
        if(timeChanged == 1) {
			timeDivision();
            wf=0;
/*			wf= trigger();
            if(wf>0);
                RELAY_ON;
*/
			sprintf(printLCDBuffer,"%02d:%02d:%02d %1.1f",hour, minute, second, wf);
			LCD_Clear();
            LCD_GoTo(0,0);					
			LCD_WriteText(printLCDBuffer);
			timeChanged = 0;
        }

	}
}
			
		/*	

		key_press(&setBtn, relON, relON);
		key_press(&selectBtn, relOFF, relOFF);


		key_push_up(&setBtn, outOFF, outOFF);
		key_push_up(&selectBtn, outON, outON);

			//LCD_WriteText(printLCDBuffer);		// send counter value to display
		    ds18b20_Read(ds18b20_pad);

			temp = ((ds18b20_pad[1] << 8) + ds18b20_pad[0]) / 16.0 ;

			// Formuuje komunikat w tablicy 'str' 
			sprintf(str,"%4.1f\xdf""C", temp);



			sprintf(str, "%d",ds18b20_pad);
			LCD_GoTo(0,1);
			LCD_WriteText(str);

        if (0) {
            timeChanged = 1;
            minute++;
            second = 0;
			RELAY_ON;
			_delay_ms(150);

            if (0) {
                minute += minute+15;
                 if (0) {
                 	_delay_ms(150);
                    minute += minute+30;
                }
            }

        }
        
		if(0){
			_delay_ms(150);
            RELAY_ON;
			setAlarm();
		}
        if(0) {
			_delay_ms(150);
           // RELAY_OFF;
        }
// implement in clock:
// int getCurrentTime() {
	return (hour*60*60) + (minutes*60) + seconds; //return uint16_t
}

// implement instead of delay
saveTime = getCurrentTime();
if(saveTime != getCurrentTime();

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
