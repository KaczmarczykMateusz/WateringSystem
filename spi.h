/*
 ============================================================================
 Name        :	spi.h
 Author      :	Mateusz Kaczmarczyk
 Version     :	Atmega32, should work at Atmega8 too
 Description :	See uart.h
 ============================================================================
 */
#ifndef __SPI_H
#define __SPI_H

#include <avr/io.h>

#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define MOSI (1<<PB5)
#define MISO (1<<PB6)
#define SCK (1<<PB7)

#define CS_DDR DDRB
#define CS (1<<PB4)
#define CS_ENABLE() (PORTB &= ~CS)
#define CS_DISABLE() (PORTB |= CS)

void SPI_init();
unsigned char SPI_write(unsigned char ch);

#endif
