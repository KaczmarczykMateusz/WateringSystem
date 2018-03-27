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
		btn->PressKeyLock = 35000;
		btn->longPressLock = LONG_PRESS_LOCK_VAL;
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
		btn->PressKeyLock = 35000;
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


/*************************************************************************
 Function: incrDcr()
 Purpose:  Incrementing and/ or decrementing time or value
 Input:    Struct with adding and subtracting buttons parameters
 	 	   Variable to be modified and its maximum allowed value
 	 	   and struct with time to be set
 Returns:  Set if time got changed
 **************************************************************************/
uint8_t incrDcr(tButton *addBtn, tButton *subtrBtn, uint8_t *modVal, uint8_t maxVal, time *modTime) {
	static uint8_t longPress = 0;
	static uint8_t shorterDelay = 0;
	uint8_t opPerformed = 0;

	register uint8_t addPressed;

	addPressed = addBtn ? (*addBtn->K_PIN & addBtn->key_mask) : 1;
	register uint8_t subtrPressed;
	subtrPressed = subtrBtn ? (*subtrBtn->K_PIN & subtrBtn->key_mask) : 1;

	if (!(addPressed && subtrPressed)) {
		uint8_t temporaryVal = 1;
		opPerformed = 1;
		if (longPress > 30) {
			temporaryVal += 60;
		} else if (longPress > 15) {
			temporaryVal += 30;
		} else if (longPress > 0) {
			temporaryVal += 3;
		}

		if(modTime && !addPressed) {
			if(temporaryVal >= 60) {
				modTime->hour ++;
			} else {
				modTime->minute += temporaryVal;
			}
			timeDivision(modTime);
		} else if(modTime && !subtrPressed) {
			if(temporaryVal < 60) {
				modTime->minute -= temporaryVal;
			}
			if(modTime->minute > 59) {
				modTime->minute = 59;
				modTime->hour --;
				if(modTime->hour > 59) {
					modTime->hour = 23;
				}
			}
			timeDivision(modTime);
		} else if(modVal && !addPressed) {
			*modVal += temporaryVal;
		} else if(modVal && !subtrPressed) {
			*modVal -= temporaryVal;
		}
		if(maxVal == 1) {			//In case if we use toggle mode
			*modVal = 0;
		} else {
			if(*modVal > maxVal) {
				*modVal = maxVal;
			}
		}


		if(!shorterDelay) {
			_delay_ms(120);
		}
		_delay_ms(120);
		if (!(addPressed && subtrPressed)) {
			shorterDelay = 1;
			if (longPress <= 26) {
				longPress++;
			}
		}
		INPUT_MODIFIED_SET;
	} else {
		longPress = 0;
		shorterDelay = 0;
	}

	if(!opPerformed) {
		return 0;
	} else {
		return 1;
	}
}
