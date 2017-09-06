/*
 ============================================================================
 Name        : main.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "main.h"


int main(void)
{
    wf = 0;

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
	time user;

	float aa;
	while (1) {

		setTime(&setBtn, &global);
/*
		key_press(&selectBtn, relOFF, relOFF);
		key_push_up(&selectBtn, relON, relON);
*/
//		key_push_up(&selectBtn, aa=getCurrentTime(&user), NULL); // not working

        if(timeChanged == 1) {
/*
 *         	key_push_up(&selectBtn, getCurrentTime(&user), NULL); // not working
			getCurrentTime(&global);
			if(saveTime != getCurrentTime());
*/
        	aa = adcOversampling();
//        	aa *= 0.000078125;
//        	aa -= ADC_CALIBRATE;
        	global.second = second;
        	timeDivision(&global);
/*          wf=0;
            wf= trigger();

            if(wf>0);
				RELAY_ON;
*/
			sprintf(printLCDBuffer,"%02d:%02d:%02d %1.3f",global.hour, global.minute, global.second, aa);
			LCD_Clear();
            LCD_GoTo(0,0);					
			LCD_WriteText(printLCDBuffer);
			timeChanged = 0;

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
