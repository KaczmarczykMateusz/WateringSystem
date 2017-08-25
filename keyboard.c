/*	keyboard.c
 *
 *      Author: Mateusz Kaczmarczyk
 */

#include "keyboard.h"


void keyboardInit(void) {
    INIT_SELECT_BTN_DDR;
    INIT_SELECT_BTN_PORT;
    INIT_SET_BTN_DDR;
    INIT_SET_BTN_PORT;
}

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
