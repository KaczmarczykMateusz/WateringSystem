/*	//TODO: implement #if for pin definitions which differ between atmega 8 and 32
 ============================================================================
 Name        :	spi.h
 Author      :	Mateusz Kaczmarczyk
 Version     :	Atmega8, Atmega32 (Just check PIN definitions)
 Description :	See uart.h
 ============================================================================
 */
#ifndef _SPI_H
#define _SPI_H

#include <avr/io.h>
#include <avr/interrupt.h>

#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define MOSI (1 << 5)
#define MISO (1 << 6)
#define SCK (1 << 7)

#define CS_DDR DDRB
#define CS (1<<4)
#define CS_ENABLE() (PORTB &= ~CS)
#define CS_DISABLE() (PORTB |= CS)

void SPI_init();
unsigned char SPI_write(unsigned char byte);
unsigned char SPI_receive();

#endif
