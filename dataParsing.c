/*
 ============================================================================
 Name        : dataParsing.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description : Parse data received via UART or other way
 ============================================================================
 */
#include "dataParsing.h"

/*************************************************************************
Function: parse_data()
Purpose:  Combine bytes into string and check them for "enter" token.
		Parsing data, searching for tokens, taking actions
Input:    Function for further parsing
Returns:  None
**************************************************************************/
//type for test AT+LED_+ON:
//type for test AT+LED_+OFF:
void parse_uart_data(char * pBuf) {
	char *parse;
	parse = strtok(pBuf, "+");
	if(!strcmp(parse, "AT")) {
		uart_puts("OK");
		if(!strcmp(parse, "LED_")) {
			if(*parse == "ON") {
				//RELAY_ON;
			} else if(*parse == "OFF") {
				//RELAY_OFF;
			}
		}
	}
}
