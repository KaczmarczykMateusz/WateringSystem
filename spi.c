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
	CS_DDR |= CS; // SD card circuit select as output
	SPI_DDR |= MOSI + SCK; // MOSI and SCK as outputs
	SPI_PORT |= MISO; // pullup in MISO, might not be needed

	// Enable SPI, master, set clock rate fck/128
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1);
}

/*************************************************************************
 Function:	SPI_write()
 Purpose:	Write one byte via SPI
 Returns:	Slave response
 **************************************************************************/
unsigned char SPI_write(unsigned char ch) {
	SPDR = ch;
	while(!(SPSR & (1<<SPIF))) {}
	return SPDR;
}
