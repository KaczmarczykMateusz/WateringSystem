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

#include "common.h"
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

uint8_t flipInteger(int8_t digit);

/** @brief:	Update text buffers for display
 *  @param:
 */
void updateTexts(control ctrlFactor, moistureCtrl mostCtrl, status sysStatus, char* (*ctrlBuff), char* (*moistCtrlBuff), char* (*sysStatusBuff));

/** @brief: Update control text buffer
 *  @param:
 */
void updateCtrlBuff(control ctrlFactor, char * (*ctrlFactorBuff));

/** @brief: Update moisture text buffer
 *  @param:
 */
void updateMoistBuff(uint8_t minMoist, char * (*moistCtrlBuff));

/** @brief: Update system status text buffer
 *  @param:
 */
void updateStatusBuff(status sysStatus, char * (*sysStatusBuff));

#endif
