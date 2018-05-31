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
 Output:	Specifies whether key was pressed
 	 	 	8: action not assigned
 **************************************************************************/
uint8_t keyPress(tButton * btn, void (*action)(void)) {
 	register uint8_t key_press = !(*btn->K_PIN & btn->key_mask);

 	if(!btn->PressKeyLock && key_press) {
		btn->PressKeyLock = 1;
	//	btn->longPressLock = LONG_PRESS_LOCK_VAL;
		if(action) {
			action(); // action for PRESS of button
			return 1;
		} else {
			return 8;
		}
	} else if(btn->PressKeyLock && !key_press) {
		btn->PressKeyLock++;
	}
 	return 0;
 }


/*************************************************************************
 Function: 	keyLongPress()
 Purpose:	assigning action to tButton key press event
 Input:		tButton struct, short and long press triggered action
 Output:	Specifies whether key was pressed
 	 	 	 0: not pressed
 	 	 	 1: short pressed (released)
 	 	 	 2: long pressed
 	 	 	 8: short press action not assigned
 	 	 	 9: long press action not assigned
 **************************************************************************/
uint8_t keyLongPress(tButton * btn, void (*shortPressAction)(void), void (*longPressAction)(void)) {
 	register uint8_t key_press = !(*btn->K_PIN & btn->key_mask);										//TODO: Try to swipe logic = !(*btn->K_PIN & btn->key_mask);

 	if(!btn->PressKeyLock && key_press) {
 		btn->longPressExecuted = 0;
		btn->PressKeyLock = 1;
		btn->longPressLock = LONG_PRESS_LOCK_VAL;
	} else if(!btn->longPressLock && key_press) {
		if(!btn->longPressExecuted && longPressAction) {
			if(longPressAction) {
				longPressAction(); // action for long PRESS of button
				btn->longPressExecuted = 1;										//TODO: change it's name to pressExecuted
				return 2;
			} else {
				return 8;
			}
		}
	} else if(btn->PressKeyLock && key_press) {
		btn->longPressLock++;
	} else if(btn->PressKeyLock && !key_press) {
		if(!btn->longPressExecuted ) {
			if(shortPressAction) {
				shortPressAction(); // action for PRESS of button
				btn->longPressExecuted = 1;
				return 1;
			} else {
				return 9;
			}
		}
		btn->PressKeyLock++;
	}
 	return 0;
 }

/*************************************************************************
 Function: 	keyPushUp()
 Purpose:	Assigning action to tButton key push up event
 Input:		tButton struct and pointer to void function performing action
 **************************************************************************/
void keyPushUp(tButton * btn, void (*action)(void)) {
	register uint8_t key_press = !(*btn->K_PIN & btn->key_mask);

 	if(!btn->PushupKeyLock && key_press) btn->PushupKeyLock = 1;
 	else if(btn->PushupKeyLock && !key_press) {
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
uint8_t incrDcr(tButton *addBtn, tButton *subtrBtn, uint32_t *modVal, uint32_t maxVal, time *modTime) {
	static uint8_t longPress = 0;
	static uint8_t shorterDelay = 0;
	uint8_t inputModified = 0;

	register uint8_t addPressed;

	addPressed = addBtn ? (*addBtn->K_PIN & addBtn->key_mask) : 1;
	register uint8_t subtrPressed;
	subtrPressed = subtrBtn ? (*subtrBtn->K_PIN & subtrBtn->key_mask) : 1;

	if (!(addPressed && subtrPressed)) {
		uint8_t temporaryVal = 1;
		if (longPress > 30) {
			temporaryVal += 60;
		} else if (longPress > 15) {
			temporaryVal += 30;
		} else if (longPress > 0) {
			temporaryVal += 3;
		}

		if(modTime && !addPressed) {	//@Purpose: Increment time
			if(temporaryVal >= 60) {
				modTime->hour ++;
			} else if(modTime) {
				modTime->minute += temporaryVal;
			}
			timeDivision(modTime);
		} else if(modTime && !subtrPressed) {	//@Purpose: Decrement time
			if(temporaryVal < 60) {
				modTime->minute -= temporaryVal;
			} else {
				modTime->hour--;
			}
			if(modTime->minute > 59) {
				modTime->minute = 59;
				modTime->hour --;
				if(modTime->hour > 23) {
					modTime->hour = 23;
				}
			}
//			timeDivision(modTime);
		} else if(modVal && !addPressed) {	//@Purpose: Increment value
			if(maxVal==1) {					//@Purpose: In case if we use toggle mode
				if(*modVal>0) {
					*modVal = 1;
				} else {
					*modVal = 0;
				}
			} else {
				if( (*modVal+temporaryVal) > maxVal ) {
					*modVal = maxVal;
				} else {
					*modVal += temporaryVal;
				}
			}
		} else if(modVal && !subtrPressed) {	//@Purpose: Decrement value
			if(maxVal==1) {			//In case if we use toggle mode
				if(*modVal>0) {
					*modVal = 1;
				} else {
					*modVal = 0;
				}
			} else {
				if(*modVal <= temporaryVal) {
					*modVal = 0;
				} else {
					*modVal -= temporaryVal;
				}
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
		inputModified = 1;
	} else {
		longPress = 0;
		shorterDelay = 0;
	}

	if(!inputModified) {
		return 0;
	} else {
		return 1;
	}
}

