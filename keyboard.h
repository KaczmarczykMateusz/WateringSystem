/*
 ============================================================================
 Name        : 	keyboard.h
 Author      : 	Mateusz Kaczmarczyk
 Version     :	Microcontroller : Atmel AVR Atmega
       			16MHz external crystal resonator
 Description :
 ============================================================================
 */
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "clock.h"

//	@brief Buttons masks definitions
#define SET_BTN_MASK 	(1<<PD3)
#define SELECT_BTN_MASK (1<<PD5)
#define STOP_BTN_MASK 	(1<<PD4)

// @brief	definitions of numbers of PINs of microcontroller for matrix keypad, just to improve readability
#define ROW_A 	7	// @brief	connect very left PIN of keyboard (closest to '1','*') to Px7 of Atmega
#define ROW_B	6
#define ROW_C 	5
#define ROW_D 	4

#define COLUMN_1	3
#define COLUMN_2	2
#define COLUMN_3	1
#define COLUMN_4	0	// @brief	connect very right PIN of keyboard (closest to 'A','D') to Px0 of Atmega

#define KEY_PAD_LOCK_VAL	1 //@brief	increasing this value decrease time between single click but may corrupt debouncing
#define LONG_PRESS_LOCK_VAL 1 //@brief	increasing this value decrease time of to pass until long press event

// @brief	struct holding button values
typedef struct {
	volatile uint8_t *K_PIN;	//@brief PINx of button
	volatile uint8_t *K_DDR;
	volatile uint8_t *K_PORT;
	uint8_t key_mask;			//@brief mask: physical PIN of microcontroller assigned tobutton
	uint8_t PressKeyLock;		//@brief overfloving counter lock of press event
	uint8_t PushupKeyLock;		//@brief overfloving counter lock of push up event
	uint8_t longPressLock;	//@brief overfloving counter lock of long press event
} tButton;

/**
 * @brief   Single button initialisation
 * 			update of Data Direction and PORT registers
 * 			initialisation of struct with button parameters
 * @param
 * @return	Struct with button parameters
 */
tButton btnInit(volatile uint8_t * K_DDR, volatile uint8_t * K_PORT, volatile uint8_t *K_PIN, uint8_t keyMask);

/**
 * @brief	Assigning action to tButton key press event
 * @param	tButton struct and pointer to void function performing action
 * 			triggered at button press event
 * @return	None
 */
void keyPress(tButton * btn, void (*action)(void));

/**
 * @brief	Assigning action to tButton key press event
 * @param	tButton struct and pointer to void function performing action
 * 			triggered at button press event
 * @return	None
 */
void keyLongPress(tButton * btn, void (*shortPressAction)(void), void (*longPressAction)(void));

/**
 * @brief	assigning action to tButton key push up event
 * @param	tButton struct and pointer to void function performing action
 * 			triggered at button push up event
 * @return	None
 */
void keyPushUp(tButton * btn, void (*action)(void));

/**
 * @brief  Setting time as 24h format by user
 * 	@param  *addBtn	:	Struct consisting of add button parameters or NULL
 *  @param  *subtrBtn:	Struct consisting of subtract button parameters or NULLL
 *  @param  *modVal	:	Struct consisting of time to be set or NULL
 *  @param	maxVal	:	Maximum value which can be reached by modVal
 *  					1: enables toggle mode where modVal ALWAYS become (0 or 1)
 *  @param	*modTime:	Variable to be incremented or NULL
 *  @return Set if time got changed, and modify time values
 *  @notice	val and tmp are interchangable
 */
uint8_t incrDcr(tButton *addBtn, tButton *subtrBtn, uint8_t *modVal, uint8_t maxVal, time *modTime);



#endif /* KEYBOARD_H */
