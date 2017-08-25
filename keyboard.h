/*	keyboard.h
 *
 *      Author: Mateusz Kaczmarczyk
 *      Microcontroller : Atmel AVR Atmega8
 *      16MHz external crystal resonator
 */
#ifndef KEYBOARD_H
#define KEYBOARD_H

#define INIT_SET_BTN_DDR DDRB &= ~(0<<PB0)
#define INIT_SET_BTN_PORT PORTB |= (1<<PB0)
#define INIT_SELECT_BTN_DDR DDRB &= ~(1<<PB1)
#define INIT_SELECT_BTN_PORT PORTB |= (1<<PB1)

//Buttons definitions
#define PIN_SET_BTN	PINB
#define SET_BTN 	(1<<PB0)

#define PIN_SELECT_BTN	PINB
#define SELECT_BTN 		(1<<PB1)

void keyboardInit(void);

typedef struct {
 volatile uint8_t *KPIN;
 uint8_t key_mask;
 void (*kfun1)(void);
 void (*kfun2)(void);
 uint8_t PressKeyLock;
 uint8_t PushupKeyLock;
 uint8_t flag;
} TBUTTON;

#endif
