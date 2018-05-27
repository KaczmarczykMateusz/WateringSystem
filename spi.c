/*
 ============================================================================
 Name        :	spi.c
 Author      :	Mateusz Kaczmarczyk
 Version     :	Atmega32, should work at Atmega8 too
 Description :	spi.h
 ============================================================================
 */

/*************************************************************************
 Function: SPI_init()
 Purpose:	Initialise Atmega SPI interface
 **************************************************************************/
#include"spi.h"

void SPI_init() {
	CS_DDR |= CS; // Cheap select as output = uC in master mode
	SPI_DDR |= MOSI | SCK; // MOSI and SCK as outputs

	// Enable SPI, master, set clock rate fck/128
	SPCR |= (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1);
}

/*************************************************************************
 Function:	SPI_write()
 Purpose:	Write one byte via SPI
 Returns:	Slave response
 **************************************************************************/
unsigned char SPI_write(unsigned char byte) {
	SPDR = byte;
	while(!(SPSR & (1<<SPIF))) {
	}
	return (SPDR);
}

/*************************************************************************
 Function:	SPI_receive()
 Purpose:	Sends one byte via SPI
 Returns:	Slave response
 **************************************************************************/
unsigned char SPI_receive() {
	SPDR = 0xFF;
	while(!(SPSR & (1<<SPIF))) {
	}
	return (SPDR);
}
