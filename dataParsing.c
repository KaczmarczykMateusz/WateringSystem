/*
 ============================================================================
 Name        : dataParsing.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description : Parse data received via UART or other way
 ============================================================================
 */
#include "dataParsing.h"

#include <stdio.h>
static const char* sysNotReadyBuff = "CZEKA";
static const char* sysReadylBuff = "GOTOWY";
static const char* sysWorkingBuff = "PRACA";
static const char* sysCheckMoistBuff = "WILG";

static const char* litreCtrlTxt = "ltr";
static const char* minuteCtrlTxt = "min";

static const char* moistOffTxt = "NIE";

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
				uartTxStr(" Turning LED ON\r\n");
				parse = strtok(NULL, "\r\n+");
				uint8_t tmpDataParse = atoi(parse);
				if(*parse == '1') {
					//TODO: implement action here and in other command fields
					uartTxStr(" 1\r\n");
				}
				if(tmpDataParse == 1) {
					//action
					uartTxStr(" true 1\r\n");
				}
			} else {
				uartTxStr("Command too short\r\n");
			}
			if(!strcmp(parse, "OFF")) {
				if(*parse == '1') {
				}
			}
		} else {
			uartTxStr("OK\r\n");
		}
	}
}

uint8_t flipInteger(int8_t digit) {
	if(digit) {
		digit *= (-1);
		digit += 100;
		return (uint8_t) digit;
	} else {
		return 100;
	}
}

/*************************************************************************
 Function: updateTexts()
 Purpose: Fill all control text buffers for LCD at once
 Input: Control factor enums and text buffers
 **************************************************************************/
void updateTexts(control ctrlFactor, uint8_t minMoist, status sysStatus, char* (*ctrlBuff), char* (*moistCtrlBuff), char* (*sysStatusBuff)) {
	updateCtrlBuff(ctrlFactor, ctrlBuff);
	updateMoistBuff(minMoist, moistCtrlBuff);
	updateStatusBuff(sysStatus, sysStatusBuff);
}

/*************************************************************************
 Function: updateCtrlBuff()
 Purpose: Fill control text buffers for LCD at once
 Input: Control factor enum and text buffer
 **************************************************************************/
void updateCtrlBuff(control ctrlFactor, char * (*ctrlFactorBuff)) {
	if(  (ctrlFactor==MINUTES) && (*ctrlFactorBuff!=minuteCtrlTxt)  ) {
		*ctrlFactorBuff= (char*)minuteCtrlTxt;
	} else if(  (ctrlFactor==LITRES) && (*ctrlFactorBuff!=litreCtrlTxt)  ) {
		*ctrlFactorBuff= (char*)litreCtrlTxt;
	}
}

/*************************************************************************
 Function: updateMoistBuff()
 Purpose: Fill control text buffers for LCD at once
 Input: Control factor enum and text buffer
 **************************************************************************/
void updateMoistBuff(uint8_t minMoist, char * (*moistCtrlBuff)) {
	static uint8_t updated = 0;
	static uint8_t minMoistHold = 0;
	if(  minMoist && (!updated|| (minMoistHold!=minMoist))) {
		sprintf(*moistCtrlBuff, "%d", minMoist);
		updated = 1;
		minMoistHold = minMoist;
	} else if(  (!minMoist) && (*moistCtrlBuff!=moistOffTxt)  ) {
		*moistCtrlBuff= (char*)moistOffTxt;
		updated = 0;
	}
}

/*************************************************************************
 Function: updateStatusBuff()
 Purpose: Fill control text buffers for LCD at once
 Input: Control factor enum and text buffer
 **************************************************************************/
void updateStatusBuff(status sysStatus, char * (*sysStatusBuff)) {
	if(  (sysStatus==READY) && (*sysStatusBuff!=&sysReadylBuff[0])  ) {
		*sysStatusBuff= (char*)sysReadylBuff;
	} else if(  (sysStatus==WORK) && (*sysStatusBuff!=&sysWorkingBuff[0])  ) {
		*sysStatusBuff= (char*)sysWorkingBuff;
	} else if(  (sysStatus==NOT_READY) && (*sysStatusBuff!=&sysNotReadyBuff[0])  ) {
		*sysStatusBuff= (char*)sysNotReadyBuff;
	} else if(  (sysStatus==CHECK_MOIST) && (*sysStatusBuff!=&sysCheckMoistBuff[0])  ) {
		*sysStatusBuff= (char*)sysCheckMoistBuff;
	}
}
