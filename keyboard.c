/*
 * keyboard.c
 *
 *  Created on: 23 Aug 2017
 *      Author: Mateusz
 */
#include "keyboard.h"



/*
void keyboardInit(void) {
//text below to header file:

//End of header file

	TBUTTON setBtn;
	setBtn.KPIN = &PIN_SET_BTN;
	setBtn.key_mask = SET_BTN;

	TBUTTON selectBtn;
	selectBtn.KPIN = &PIN_SET_BTN;
	selectBtn.key_mask = SET_BTN;
}
*/
void key_press( TBUTTON * btn, void (*action1)(void), void (*action2)(void) ) {
 	register uint8_t key_press = (*btn->KPIN & btn->key_mask);

 	if( !btn->PressKeyLock && !key_press ) {
		btn->PressKeyLock=1;
		if(action1) action1(); // action for PRESS of button
		if(action2) action2();

	} else if( btn->PressKeyLock && key_press ) (++btn->PressKeyLock);
 }

void key_push_up( TBUTTON * btn, void (*action1)(void), void (*action2)(void) ) {
	register uint8_t key_press = (*btn->KPIN & btn->key_mask);

 	if( !btn->PushupKeyLock && !key_press ) btn->PushupKeyLock = 1;
 	else if( btn->PushupKeyLock && key_press ) {
  		if( !++btn->PushupKeyLock ) {
  			if(action1) action1();   //action for PUSH_UP (button release)
			if(action2) action2();
		}
	}
}



