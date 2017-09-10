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
    wf = 0;

	OUTPUT_PIN_A111_INIT;
	RELAY_INIT;
    OUTPUT_PIN_A111_ON;

//	powerSaveInit();
    keyboardInit();
	rt_clock_init();
	LCD_Initalize();
	adcInitAvcc();

    tButton setBtn;
    tButton selectBtn;
	setBtn = setBtnInit();
	selectBtn = selectBtnInit();

	time global;
//	time user;

	double temporary;
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
//        	temporary = (double)measureMoisture();
        	temporary = (double)lightAsPercent();
        	global.second = second;
        	timeDivision(&global);
/*          wf=0;
            wf= trigger();

            if(wf>0);
				RELAY_ON;
*/
			sprintf(printLCDBuffer,"%02d:%02d:%02d %1.5f",global.hour, global.minute, global.second, (double)temporary);
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
