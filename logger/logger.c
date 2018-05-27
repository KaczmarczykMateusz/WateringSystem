/*
 ============================================================================
 Name        : logger.c		//TODO: split structure of logger into smaller functions
 Author      : Mateusz Kaczmarczyk
 Version     : Atmega8
 Description : See header file, important.
 ============================================================================
 */

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/eeprom.h>

#include "logger.h"

#include <util/delay.h>

//@Brief: define the logger eeprom structure
typedef struct {
	uint8_t EEPROM_Init;
    uint8_t fileNumber;
    uint16_t filePosition;	// TODO: implement resuming from current position not only current file
} logger_eet;
logger_eet EEMEM EEPROM_Address;
logger_eet EEPROM_var;

volatile uint8_t loggerErrorOpenWrite = 0;


/*************************************************************************
 Function: writeEEPROM()
 Purpose:	Save current file position to EEPROM
 Input:    * 0: function exit with no action
 	 	   * 1: confirmation that we should write to EEPROM
 **************************************************************************/
void writeEEPROM(uint8_t isEEPROM_SaveTime) {
	if(isEEPROM_SaveTime) {
		//write actual variables to EEPROM
		eeprom_write_block((const void*)&EEPROM_var, (void*)&EEPROM_Address, sizeof(logger_eet));
		//reset errors
		if(loggerErrorOpenWrite == LOGGER_MAX_LOGGING_ERRORS) {
			loggerErrorOpenWrite = 0;
		}
	}
}

/*************************************************************************
 Function: logerInit()
 Purpose: Initialize logger and it's EEPROM memory
 **************************************************************************/
void logerInit() {
	eeprom_read_block((void*)&EEPROM_var, (const void*)&EEPROM_Address, sizeof(logger_eet));

	//@Purpose: Init eeprom if not done before
	if(((uint32_t)EEPROM_var.EEPROM_Init & 0XFF) == 0xFF) {
		EEPROM_var.EEPROM_Init = 1;
		EEPROM_var.fileNumber = 1;
		EEPROM_var.filePosition = 0;
		eeprom_write_block((const void*)&EEPROM_var, (void*)&EEPROM_Address, sizeof(logger_eet));
	}

#if LOGGER_ERROR_LED
	//@Purpose: Init led signaling log error
	LED_LOG_ERR_DDR |= (1<<LED_LOG_ERR_PIN); //output
	LED_LOG_ERR_PORT &= ~(1<<LED_LOG_ERR_PIN); //off
#endif
}

/*************************************************************************
 Function: loggerLog()
 Purpose: log data
 Input:  logbuf[]: buffer to save to SD card
 Return: @See: LOGGER_FLAGS
 **************************************************************************/
uint8_t loggerLog(char logbuf[]) {
	static uint8_t loggerOpenNextFile = 0;
	static uint8_t logger_mount = 0;
	static uint8_t loggerIsFileOpened = 0;
	static FATFS FatFs;
	uint16_t logbufsize = (uint16_t)strlen(logbuf);
	uint8_t ret = LOGGER_NOT_READY;

	char filelog[12] = "";
	char filenum[3] = "";

	WORD bw;

	if(loggerOpenNextFile) {
		#if LOGGER_DEBUG
		uartTxStr("opening next\r\n");
		#endif

		disk_initialize();

		loggerOpenNextFile = 0;

		logger_mount = 0;
		loggerIsFileOpened = 0;

		EEPROM_var.filePosition = 0;
		EEPROM_var.fileNumber = EEPROM_var.fileNumber + 1;
		if(EEPROM_var.fileNumber%(LOGGER_FILE_AMOUNT+1) == 0) {
			EEPROM_var.fileNumber = 1;
		}

		eeprom_write_block((const void*)&EEPROM_var, (void*)&EEPROM_Address, sizeof(logger_eet));
	}

	if(loggerErrorOpenWrite != LOGGER_MAX_LOGGING_ERRORS) {

		if(!logger_mount) {
			if(pf_mount(&FatFs) == FR_OK) {
				logger_mount = 1;
			} else {
				#if LOGGER_DEBUG
				uartTxStr("mount: error\r\n");
				//uartTxInt();
				#endif
				ret = LOGGER_MOUNT_ERR;
				loggerErrorOpenWrite++;
				//@Purpose: If max errors reached try open next file
				if(loggerErrorOpenWrite == LOGGER_MAX_LOGGING_ERRORS) {
					loggerOpenNextFile = 1;
				}
			}
		}

		if(logger_mount) {
			//@Purpose: Get current file name
			memset(&filelog[0], 0, sizeof(filelog));
			memset(&filenum[0], 0, sizeof(filenum));
			if(EEPROM_var.fileNumber > LOGGER_FILE_AMOUNT)
				EEPROM_var.fileNumber = 1;
			itoa(EEPROM_var.fileNumber, filenum, 10);
			strcpy (filelog,"log");
			if(EEPROM_var.fileNumber < 10) //do padding
				strcat(filelog,"0");
			strcat(filelog,filenum);
			strcat(filelog,".log");

			#if LOGGER_DEBUG
			uartTxStr("> ");
			uartTxStr(filelog);
			uartTxStr("\r\n");
			#endif

			if(!loggerIsFileOpened) {
				if (pf_open(filelog)== FR_OK) {
					if(FR_OK == pf_lseek(EEPROM_var.filePosition)) {
						loggerIsFileOpened = 1;
						loggerErrorOpenWrite = 0;
					} else {
						#if LOGGER_DEBUG
						uartTxStr("seek next sector: error\r\n");
						#endif
					}
				} else {
					#if LOGGER_DEBUG
					uartTxStr("open: error\r\n");
					#endif

					loggerErrorOpenWrite++;
					//@Purpose: If max errors reached try open next file
					if(loggerErrorOpenWrite == LOGGER_MAX_LOGGING_ERRORS) {
						loggerOpenNextFile = 1;
					}
				}
			}

			if(loggerIsFileOpened) {
				if(EEPROM_var.filePosition+logbufsize<LOGGER_FILE_SIZE) {
					//@Purpose: Write data to the file
					pf_write(logbuf, (WORD)logbufsize, &bw);
					if (bw != (WORD)logbufsize) {
						#if LOGGER_DEBUG
						uartTxStr("write: error\r\n");
#if LOGGER_ERROR_LED
						LED_LOG_ERR_PORT |= (1<<LED_LOG_ERR_PIN);
#endif
						#endif

						loggerErrorOpenWrite++;
						//if max errors reached try open next file
						if(loggerErrorOpenWrite == LOGGER_MAX_LOGGING_ERRORS) {
							loggerOpenNextFile = 1;
							ret = LOGGER_NEXT_FILE;
						} else {
							ret = LOGGER_WRITE_ERR;
						}
					} else {
						#if LOGGER_DEBUG
						uartTxStr("write: ok\r\nFile pointer: ");
						uartTxInt(FatFs.fptr);
						uartTxStr("\r\n");
#if LOGGER_ERROR_LED
						LED_LOG_ERR_PORT &= ~(1<<LED_LOG_ERR_PIN); // LED OFF
#endif
						#endif
						loggerErrorOpenWrite = 0;

						EEPROM_var.filePosition += logbufsize;
						ret = LOGGER_SUCCESS;
						//saveFileAndContinue(FatFs);

					}
				} else {
					#if LOGGER_DEBUG
					uartTxStr("write: file full, move to next\r\n");
					#endif

					//try to write blank data until the end of the file
					uint16_t i = 0;
					for(i=0; i<LOGGER_FILE_SIZE-EEPROM_var.filePosition; i++) {
						pf_write(" ", (WORD)1, &bw);
					}

					loggerOpenNextFile = 1;
				}
			}
		}
	}
	return ret;
}

/*************************************************************************
 Function: saveFileAndContinue()
 Purpose:  Flush in order to save, and move file pointer to next sector
 Input:    none
 See: 	   If use this function add additional padding (\r\n) to string
 	 	   Use this function only if really needed because it file size grow
 	 	   at much faster rate (ratio depends on length of your buffer)
 **************************************************************************/
void saveFileAndContinue(FATFS fatObj) {
	DWORD resumeFrom = fatObj.fptr;			//Save file pointer
	pf_write(NULL, 0, NULL);
	pf_lseek(resumeFrom+512);				//Move file pointer to next sector
}

uint8_t updateLogger(time _time, value _sensVal) {
	char logger_buffer[LOGGER_BUFF_LENGTH];
	char buf[10];

	//build logger_line
	memset(logger_buffer, 0, sizeof(logger_buffer));

	strcat(logger_buffer, "Day:");
	itoa(_time.day, buf, 10); strcat(logger_buffer, buf);
	strcat(logger_buffer, " ");
	itoa(_time.hour, buf, 10); strcat(logger_buffer, buf);
	strcat(logger_buffer, ":");
	itoa(_time.minute, buf, 10); strcat(logger_buffer, buf);
	strcat(logger_buffer, ":");
	itoa(_time.second, buf, 10); strcat(logger_buffer, buf);

	strcat(logger_buffer, " T:");	itoa(_sensVal.temp, buf, 10);	strcat(logger_buffer, buf);
	//strcat(logger_buffer, ".");	itoa(temp.tempFract, buf, 10);	strcat(logger_buffer, buf);
	strcat(logger_buffer, " L:");	itoa(_sensVal.bright, buf, 10);	strcat(logger_buffer, buf);
	strcat(logger_buffer, " M:");	itoa(_sensVal.moisture, buf, 10);	strcat(logger_buffer, buf);
	strcat(logger_buffer, " WA:");	itoa(_sensVal.wfVolume, buf, 10);	strcat(logger_buffer, buf);
	strcat(logger_buffer, "\r\n");

	//log logger_line
	#if LOGGER_DEBUG
	uartTxStr("logging...\r\n");
	uartTxStr("*******\r\n");
	uartTxStr(logger_buffer);
	uartTxStr("*******\r\n");
	#endif

	uint8_t ret = loggerLog(logger_buffer);

	return ret;

}
