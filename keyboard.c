/*
 ============================================================================
 Name        : keyboard.c
 Author      : Mateusz Kaczmarczyk
 Version     : Microcontroller : Atmel AVR Atmega32
 Description : See header file
 ============================================================================
 */

#include "keyboard.h"
#include <avr/io.h>

/*************************************************************************
 Function: 	btnInit()
 Purpose:	Initialise single button
 Input:		Parameters of button connection; DDR, PORT, PIN adresses and key mask
 Returns:	Returns struct with button parameters
 **************************************************************************/
tButton btnInit(volatile uint8_t * K_DDR, volatile uint8_t * K_PORT, volatile uint8_t *K_PIN, uint8_t keyMask) {
	tButton setBtn;
//	setBtn.K_DDR &= ~keyMask;
//	setBtn.K_PORT |= keyMask;
	setBtn.K_DDR = K_DDR;
	setBtn.K_PORT = K_PORT;
	setBtn.K_PIN = K_PIN;
	setBtn.key_mask = keyMask;
	setBtn.PressKeyLock = 0;
	*setBtn.K_DDR &= ~keyMask;
	*setBtn.K_PORT |= keyMask;

	return setBtn;
}

/*************************************************************************
 Function: 	key_press()
 Purpose:	assigning action to tButton key press event
 Input:		tButton struct and pointer to void function performing action
 **************************************************************************/
void keyPress(tButton * btn, void (*action)(void)) {
 	register uint8_t key_press = (*btn->K_PIN & btn->key_mask);

 	if(!btn->PressKeyLock && !key_press) {
		btn->PressKeyLock = 1;
		if(action) {
			action(); // action for PRESS of button
		}

	} else if(btn->PressKeyLock && key_press) {
		(++btn->PressKeyLock);
	}
 }

/*************************************************************************
 Function: 	keyLongPress()
 Purpose:	assigning action to tButton key press event
 Input:		tButton struct, short and long press triggered action
 **************************************************************************/
void keyLongPress(tButton * btn, void (*shortPressAction)(void), void (*longPressAction)(void)) {
 	register uint8_t key_press = (*btn->K_PIN & btn->key_mask);

 	if(!btn->PressKeyLock && !key_press) {
		btn->PressKeyLock = 1;
		btn->longPressLock = LONG_PRESS_LOCK_VAL;
		if(shortPressAction) {
			shortPressAction(); // action for PRESS of button
		}
	}  else if(btn->PressKeyLock && key_press) {
		(++btn->PressKeyLock);
	} else if(!key_press && btn->longPressLock) {
		btn->longPressLock++;
	} else if(!btn->longPressLock && !key_press) {
		if(longPressAction) {
			longPressAction(); // action for PRESS of button
		}
	}

 }

/*************************************************************************
 Function: 	keyPushUp()
 Purpose:	Assigning action to tButton key push up event
 Input:		tButton struct and pointer to void function performing action
 **************************************************************************/
void keyPushUp(tButton * btn, void (*action)(void)) {
	register uint8_t key_press = (*btn->K_PIN & btn->key_mask);

 	if(!btn->PushupKeyLock && !key_press) btn->PushupKeyLock = 1;
 	else if(btn->PushupKeyLock && key_press) {
  		if(!++btn->PushupKeyLock) {
  			if(action) {
  				action();   //action for PUSH_UP (button release)
  			}
		}
	}
}
