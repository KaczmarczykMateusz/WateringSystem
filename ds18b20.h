/*
 ============================================================================
 Name        : ds18b20.h
 Author      : Mateusz Kaczmarczyk
 Version     : Atmel AVR Atmega32
 Description : One wire protocole for single ds18b20 sensor without CRC
			   !!! Include delays
 ============================================================================
*/
#ifndef DS18B20_H
#define DS18B20_H

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>


#define THERM_DDR	DDRD
#define THERM_PORT	PORTD
#define THERM_PIN	PIND
#define THERM_MASK	(1 << 7)

#define SET_ONEWIRE_PORT	THERM_PORT	|= THERM_MASK
#define CLR_ONEWIRE_PORT	THERM_PORT	&= ~THERM_MASK
#define ONEWIRE_PIN_CHECK	THERM_PIN	& THERM_MASK
#define ONEWIRE_OUT			THERM_DDR	|= THERM_MASK
#define ONEWIRE_IN			THERM_DDR	&= ~THERM_MASK

uint8_t thermPad[8];

typedef struct {
	uint32_t tempInt;		// @brief Integer part of temperature result
	uint32_t tempFract;		// @brief Fractional part of temperature result
	uint32_t tempMultip;	// @brief temperature multiplied by (*100)
} TEMP;

/**
 * @brief	Reset sensor and return success or failure
 * @param	None
 * @return	Success ('1') or failure ('0')
 */
uint8_t oneWireReset(void);

/**
 * @brief	Write single bit to sensor
 * @param	1: bit to set
 	 	 	0: bit to clear
 * @return	None
 */
void oneWireWriteBit(uint8_t bit);

/**
 * @brief	Read single bit from sensor
 * @param	None
 * @return	1: bit set
 	 	 	0: bit clear
 */
uint8_t oneWireReadBit(void);

/**
 * @brief	Send single byte to temperature sensor
 * @param
 * @return	None
 */
void oneWireWriteByte(uint8_t);

/**
 * @brief	Send command to convert temperature in sensor
 * 			!!! Required delay after function
 * 			exits is 750ms
 * @param	None
 * @return	Success ('1') or failure ('0')
 */
uint8_t ds18b20_ConvertT(void);

/**
 * @brief	Read single byte from sensor
 * @param	None
 * @return	Received byte
 * @see		Use it in ds18b20_Read()
 */
uint8_t oneWireReadByte(void);

/**
 * @brief	Reads data from sensor after conversion into array
 * @param	Reads byte by byte into array :
 * 			LSB (Least Significant Byte) (x[0]): divided by 16
 * 			shows up as integer part of Celsius
 * 			One older byte (LSB + 1) (x[1]): holds temperature
 * 			which divided by 16 shows up as fraction of Celsius
 * @return	Success ('1') or failure ('0')
 */
uint8_t ds18b20_Read(uint8_t []);

/**
 * @brief	Fills provided struct with results from sensor
 * 			!!! Call this function minimum 750ms after ds18b20_ConvertT()
 * @param	Struct with temperature values ready to be filled by function
 * @return	None
 */
void readTemperature(TEMP *);

#endif
