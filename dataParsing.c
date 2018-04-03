/*
 ============================================================================
 Name        : dataParsing.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description : Parse data received via UART or other way
 ============================================================================
 */
#include "dataParsing.h"

static const char* sysNotReadyBuff = "CZEKA";
static const char* sysReadylBuff = "GOTOWY";
static const char* sysWorkingBuff = "PRACA";
static const char* sysCheckTimeTempBuff = "OPOZN";
static const char* sysCheckMoistBuff = "WILG";

static const char* litreCtrlTxt = "ltr";
static const char* minuteCtrlTxt = "min";

static const char* moistOnTxt = "TAK";
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
void updateTexts(control ctrlFactor, moistureCtrl mostCtrl, status sysStatus, char* (*ctrlBuff), char* (*moistCtrlBuff), char* (*sysStatusBuff)) {
	updateCtrlBuff(ctrlFactor, ctrlBuff);
	updateMoistBuff(mostCtrl, moistCtrlBuff);
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
void updateMoistBuff(moistureCtrl moistCtrl, char * (*moistCtrlBuff)) {
	if(  (moistCtrl==MOIST_ON) && (*moistCtrlBuff!=&moistOnTxt[0])  ) {
		*moistCtrlBuff= (char*)moistOnTxt;
	} else if(  (moistCtrl==MOIST_OFF) && (*moistCtrlBuff!=moistOffTxt)  ) {
		*moistCtrlBuff= (char*)moistOffTxt;
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
	} else if(  (sysStatus==CHECK_TIME_AND_TEMP) && (*sysStatusBuff!=&sysCheckTimeTempBuff[0])  ) {
		*sysStatusBuff= (char*)sysCheckTimeTempBuff;
	} else if(  (sysStatus==CHECK_MOIST) && (*sysStatusBuff!=&sysCheckMoistBuff[0])  ) {
		*sysStatusBuff= (char*)sysCheckMoistBuff;
	}
}
