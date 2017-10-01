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
#include <math.h>

#define SET_ONEWIRE_PORT     PORTD |=  _BV(7)
#define CLR_ONEWIRE_PORT     PORTD  &= ~_BV(7)
#define IS_SET_ONEWIRE_PIN   PIND   &   _BV(7)
#define SET_OUT_ONEWIRE_DDR  DDRD   |=  _BV(7)
#define SET_IN_ONEWIRE_DDR   DDRD   &= ~_BV(7)

typedef struct {
	uint32_t tempInt;		// @brief Integer part of temperature result
	uint32_t tempFract;		// @brief Fractional part of temperature result
	uint32_t tempMultip;	// @brief temperature multiplied by (*100)
} TEMP;

unsigned char ds18b20_ConvertT(void);
int ds18b20_Read(unsigned char []);
void OneWireStrong(char);
unsigned char OneWireReset(void);
void OneWireWriteByte(unsigned char);
unsigned char OneWireReadByte(void);
void readTemperature(TEMP * tempe);

unsigned char ds18b20_pad[9];


#endif
