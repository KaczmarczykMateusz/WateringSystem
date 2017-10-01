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
Function: parse_uart_data(char * pBuf)
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

/*************************************************************************
Function: sendInteger(toChar passed)
Purpose:  Sends integer decoded as char
		  For use with user interface
Input:	  Digit to send, not larger than 999
Returns:  None
**************************************************************************/
void sendInteger(uint16_t passedValue) {
	char auxChar;
	char auxCharTemp = 'a';
	if(passedValue <= 999) {
		if(passedValue > 99) {
			auxCharTemp= (passedValue % 10) + '0';
			passedValue /= 10;
			}
		if(passedValue > 9) {
			auxChar = (passedValue / 10) +'0';
			uartPutc(auxChar);
		}
		auxChar = (passedValue % 10) +'0';
		uartPutc(auxChar);
		if(auxCharTemp != 'a') {
				uartPutc(auxCharTemp);
		}
		uartPuts(" ");
	}
}
