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
	parse = strtok(pBuf, "\r+");
	if(!strcmp(parse, "AT")) {
		parse = strtok(NULL, "\r+");
		if(!strcmp(parse, "LED")) {
			parse = strtok(NULL, "\r+");
			if(!strcmp(parse, "ON")) {
				uartPuts(" Turning LED ON\r\n");
				parse = strtok(NULL, "\r\n+");
				uint8_t tmpDataParse = atoi(parse);
				if(*parse == '1') {
					//TODO: implement action here and in other command fields
					uartPuts(" 1\r\n");
				}
				if(tmpDataParse == 1) {
					//action
					uartPuts(" true 1\r\n");
				}
			} else {
				uartPuts("Command too short\r\n");
			}
			if(!strcmp(parse, "OFF")) {
				if(*parse == '1') {
				}
			}
		} else {
			uartPuts("OK\r\n");
		}
	}
}
