/*	ds18b20.h
 *
 *      Author: Mateusz Kaczmarczyk
 *      Microcontroller : Atmel AVR Atmega8
 *      16MHz external crystal resonator
 *      1 wire protocole
 *      basic version
 */

#ifndef DS18B20_H
#define DS18B20_H

#include <avr/io.h>
#include <util/delay.h>

#define SET_ONEWIRE_PORT     PORTC |=  _BV(5)
#define CLR_ONEWIRE_PORT     PORTC  &= ~_BV(5)
#define IS_SET_ONEWIRE_PIN   PINC   &   _BV(5)
#define SET_OUT_ONEWIRE_DDR  DDRC   |=  _BV(5)
#define SET_IN_ONEWIRE_DDR   DDRC   &= ~_BV(5)

unsigned char ds18b20_ConvertT(void);
int ds18b20_Read(unsigned char []);
void OneWireStrong(char);
unsigned char OneWireReset(void);
void OneWireWriteByte(unsigned char);
unsigned char OneWireReadByte(void);


#endif
