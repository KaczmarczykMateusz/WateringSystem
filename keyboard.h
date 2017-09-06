/*	keyboard.h
 *
 *      Author: Mateusz Kaczmarczyk
 *      Microcontroller : Atmel AVR Atmega
 *      16MHz external crystal resonator
 */
#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#define INIT_SET_BTN_DDR DDRD &= ~(1<<PD6)
#define INIT_SET_BTN_PORT PORTD |= (1<<PD6)
#define INIT_SELECT_BTN_DDR DDRD &= ~(1<<PD5)
#define INIT_SELECT_BTN_PORT PORTD |= (1<<PD5)

//Buttons definitions
#define PIN_SET_BTN	PIND
#define SET_BTN 	(1<<PD6)

#define PIN_SELECT_BTN	PIND
#define SELECT_BTN 		(1<<PD5)

typedef struct _TBUTTON{
 volatile uint8_t *KPIN;
 uint8_t key_mask;
 void (*kfun1)(void);
 void (*kfun2)(void);
 uint8_t PressKeyLock;
 uint8_t PushupKeyLock;
 uint8_t flag;
} tButton;

void keyboardInit(void);

tButton setBtnInit();
tButton selectBtnInit();

void key_press( tButton * btn, void (*action1)(void), void (*action2)(void) );

void key_push_up( tButton * btn, void (*action1)(void), void (*action2)(void) );




#endif
