/*
 ============================================================================
 Name        : logger.h
 Author      : Mateusz Kaczmarczyk
 Version     : Microcontroller : Atmel AVR Atmega8
 Description :	In logger case flush to disk is done every written sector (i.e. 512byte) because it can't perform full
				writing operation every time  as far as data wouldn't be packed enough. Every full write operation starts
				from new cluster.

				Using petitFs library remember about it's limitations :
				* Cannot create file. Only existing file can be written.
				* Cannot expand file size.
				* Cannot update time stamp of the file.
				* Write operation can start/stop on the sector boundary only.
				* Read-only attribute of the file cannot block write operation.

				Disk has to be mounted and file opened prior to writing operation
				Generally (not exactly in our case) file write operation must be done in following sequence.:
				* pf_lseek(ofs);
				* pf_write(buff, btw, &bw); Any other file function cannot be used while a write operation is in progress.
				* pf_write(0, 0, &bw); Finalize the current write operation.
				 	 If read/write pointer is not on the sector boundary, left bytes in the sector will be filled with zero.
				 	 Logger doesn't finalize every operation because there would be too much space left
				 	 empty (single log is much less than 512byte(sector size in most cases))
 ============================================================================
 */
#ifndef LOGGER_H_
#define LOGGER_H_
#include "../common.h"
#include "pfatfs/pff.h"
#include "pfatfs/diskio.h"

#define LOGGER_FILE_AMOUNT 10		//Define maximum number of logger files
#define LOGGER_BUFF_LENGTH 100		//Define maximum length for logger buffer

#define LOGGER_FILE_SIZE 50000 		//File size in byte (do not exceed 2^16)
#define LOGGER_MAX_LOGGING_ERRORS 20 //Max number of errors, go to next file if max errors occur

//@Brief: LOGGER_FLAGS
#define LOGGER_NOT_READY	0
#define LOGGER_MOUNT_ERR	1
#define LOGGER_WRITE_ERR	2
#define LOGGER_NEXT_FILE	3
#define LOGGER_SUCCESS 		4

#define LOGGER_DEBUG 0 				//Enable or disable debug
#define LOGGER_ERROR_LED 0			//Enable or disable led signaling error
#if LOGGER_DEBUG
#include "../uart.h"
//@Brief: LED signaling log error definitions
#define LED_LOG_ERR_DDR DDRD
#define LED_LOG_ERR_PORT PORTD
#define LED_LOG_ERR_PIN PD7
#endif


/**
 * @brief:
 * @param:	None
 */
void writeEEPROM(uint8_t isEEPROM_SaveTime);

/**
 * @brief: Initialize logger with its EEPROM
 */
void logerInit();

/**
 * @brief:	Write string to SD card
 * @param:	buffer to write at SD card
 * @return: binary value determining failure or success
 */
uint8_t loggerLog(char logbuf[]);

/**
 * @brief: Save file and move it's pointer to next sector
 * to prepare for next writing (prevent overwriting)
 * @param:	buffer to write at SD card
 */
void saveFileAndContinue(FATFS fatObj);

uint8_t updateLogger(time _time, value _sensVal);

#endif
