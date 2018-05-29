/*
 ============================================================================
 Name        : ds18b20.c
 Author      : Mateusz Kaczmarczyk
 Version     : Atmel AVR Atmega32
 	 	 	   !!! Include delays
 Description : Resources http://www.jaglarz.info/ja/elektronika/ds18b20.pdf
 	 	 	   Access from 04.10.2017

 	 	 	   Communication order flow:
 	 	 	   - Initialisation,
 	 	 	   - ROM commands,
 	 	 	   - Functions
 	 	 	   Except Search ROM and Alarm SEARCH where master has to return
 	 	 	   to first step after sending those commands

 	 	 	   Sensor need external pull up resistor on BUS line
 ============================================================================
 */

#include "ds18b20.h"

/*************************************************************************
 Function: 	oneWireReset()
 Purpose:	Reset sensor before sending further commands
 Returns:	Success ('1') or failure ('0')
 **************************************************************************/
uint8_t oneWireReset() {
	CLR_ONEWIRE_PORT;			// Normal input no pull up

	if(!(ONEWIRE_PIN_CHECK)) {	// detect 0V on buss error
		return 0;
	}

	ONEWIRE_OUT;
	_delay_us(500);				// Reset command pulling low 500ms
	ONEWIRE_IN;					// Set to input
	_delay_us(70);				// Wait for "presence pulse"

	if(!(ONEWIRE_PIN_CHECK)) {	// Check if slave present at BUS
		_delay_us(500);
		return(1);				// Ready for ROM commands
	}
	_delay_us(500);

	return(0);
}

/*************************************************************************
 Function: 	oneWireWriteBit()
 Purpose:	Write single bit to sensor
 Input:		1: bit to set
 	 	 	0: bit to clear
 **************************************************************************/
void oneWireWriteBit(uint8_t bit) {
	ONEWIRE_OUT;

	if(bit & 0x01) {	// Send 1
		_delay_us(7);
		ONEWIRE_IN;
		_delay_us(70);
	} else {			// Send 0
		_delay_us(70);
		ONEWIRE_IN;
		_delay_us(7);
	}
}

/*************************************************************************
 Function: 	oneWireReadBit()
 Purpose:	Read single bit from sensor
 Returns:	1: bit set
 	 	 	0: bit clear
 **************************************************************************/
uint8_t oneWireReadBit(void) {
	uint8_t bit = 0;
	ONEWIRE_OUT;		// Set output to '0
	_delay_us(7);
	ONEWIRE_IN;			// Set to input
	_delay_us(7);

	if(ONEWIRE_PIN_CHECK) {
		bit |= 0x80;
	}

	_delay_us(70);

	return bit;
}

/*************************************************************************
 Function: 	oneWireWriteByte()
 Purpose:	Write single byte to sensor
 Input:		Byte to send
 **************************************************************************/
void oneWireWriteByte(uint8_t byte)
{
	uint8_t i;
	CLR_ONEWIRE_PORT;

	for (i=0; i<8; i++) {
		oneWireWriteBit(byte);
		byte >>= 1;
	}
}


/*************************************************************************
 Function: 	oneWireReadByte()
 Purpose:	Read single byte from sensor
 Returns:	Single byte received from sensor
 **************************************************************************/
uint8_t oneWireReadByte(void) {
	uint8_t i, byte = 0;

	ONEWIRE_IN;				// Output '0' or input without pullup

	for(i=0; i<8; i++) {
		if(oneWireReadBit()) {
			byte |= (1<<i);
		}
	}
	return byte;
}

/*************************************************************************
 Function: 	ds18b20_ConvertT()
 Purpose:	Convert temperature, prepare it to be sent (needs 750ms to process)
 Returns:	Success ('1') or failure ('0')
 **************************************************************************/
uint8_t ds18b20_ConvertT(void) {
	if(!oneWireReset()) {
		return 0;
	}
	//oneWireWriteByte(0x55); // MATCH ROM
	oneWireWriteByte(0xCC); // SKIP ROM
	oneWireWriteByte(0x44); // CONVERT T

	return 1;
}

void ds18b20_Search_ROM(void) {
	oneWireWriteByte(0xF0);	// Search rom (reads ROMs of all slaves)
	//oneWireWriteByte(0x33); // Read rom (reads ROMs only in case if single slave at BUS)
}


/*************************************************************************
 Function: 	ds18b20_Read()
 Purpose:	Read entire message from sensor scratchpad
 Input:		Array to input schratchpad data into
 Returns:	Success ('1') or failure ('0')
 **************************************************************************/
uint8_t ds18b20_Read(uint8_t scratchpad[]) {
	uint8_t i;

	if(!oneWireReset()) return 0;
	//oneWireWriteByte(0x55); // MATCH ROM
	oneWireWriteByte(0xCC); // SKIP ROM

	oneWireWriteByte(0xbe); // READ SCRATCHPAD

	for(i=0; i<9; i++) {
		scratchpad[i] = oneWireReadByte();
	}

	return 1;
}

/*************************************************************************
 Function: 	readTemperature()
 Purpose:	Read and convert temperature from earlier obtained from scratchpad array
 Input:		Struct holding temperature variables
 Returns:	None, but modifies values in the struct
 **************************************************************************/
void readTemperature(TEMP * tempe) {
	ds18b20_Read(thermPad);
	tempe->tempMultip = (thermPad[1] << 8) + thermPad[0];
	tempe->tempMultip *= 100;
	tempe->tempMultip = tempe->tempMultip >> 4;
	tempe->tempInt =  tempe->tempMultip / 100;

	tempe->tempFract = tempe->tempMultip - (tempe->tempInt * 100);
	tempe->tempFract = round(tempe->tempFract / 10.0f);
}

