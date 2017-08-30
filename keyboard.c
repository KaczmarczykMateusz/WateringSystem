/*	keyboard.c
 *
 *      Author: Mateusz Kaczmarczyk
 */

#include "keyboard.h"
#include <avr/io.h>

typedef struct _TBUTTON{
 volatile uint8_t *KPIN;
 uint8_t key_mask;
 void (*kfun1)(void);
 void (*kfun2)(void);
 uint8_t PressKeyLock;
 uint8_t PushupKeyLock;
 uint8_t flag;
} tButton;

void keyboardInit(void) {
    INIT_SELECT_BTN_DDR;
    INIT_SELECT_BTN_PORT;
    INIT_SET_BTN_DDR;
    INIT_SET_BTN_PORT;
}

void key_press( tButton * btn, void (*action1)(void), void (*action2)(void) ) {
 	register uint8_t key_press = (*btn->KPIN & btn->key_mask);

 	if( !btn->PressKeyLock && !key_press ) {
		btn->PressKeyLock=1;
		if(action1) action1(); // action for PRESS of button
		if(action2) action2();

	} else if( btn->PressKeyLock && key_press ) (++btn->PressKeyLock);
 }

void key_push_up( tButton * btn, void (*action1)(void), void (*action2)(void) ) {
	register uint8_t key_press = (*btn->KPIN & btn->key_mask);

 	if( !btn->PushupKeyLock && !key_press ) btn->PushupKeyLock = 1;
 	else if( btn->PushupKeyLock && key_press ) {
  		if( !++btn->PushupKeyLock ) {
  			if(action1) action1();   //action for PUSH_UP (button release)
			if(action2) action2();
		}
	}
}

