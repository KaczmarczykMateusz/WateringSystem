/*
 ============================================================================
 Name        : dataParsing.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description : Parse data received via UART or other way
 ============================================================================
 */
#include <string.h>
#include <stdlib.h>

#include "uart.h"

/**
   @brief   Parse data received via UART, check if they match AT commands standard
   @param   buffer with data received from UART (until first 'Enter')
   @return  none
*/
void parse_uart_data(char * pBuf);

