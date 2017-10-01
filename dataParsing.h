/*
 ============================================================================
 Name        : dataParsing.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description : Parse data received via UART or other way
 ============================================================================
 */
#ifndef DATA_PARSING_H
#define DATA_PARSING_H
#include <string.h>
#include <stdlib.h>

#include "uart.h"

typedef struct{
	uint8_t baseInteger;
	uint8_t firstPart;
	uint8_t secondPart;
	uint8_t thirdPart;
}toChar;

/**
   @brief   Parse data received via UART, check if they match AT commands standard
   @param   buffer with data received from UART (until first 'Enter')
   @return  none
*/
void parse_uart_data(char *pBuf);

/**
   @brief   TODO
   @param
   @return  none
*/
void sendInteger(uint16_t passedValue);

#endif
