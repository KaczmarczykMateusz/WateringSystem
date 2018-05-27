/*
 ============================================================================
 Name        : action.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#ifndef ACTION_H
#define ACTION_H

#include <stdio.h>
#include <avr/io.h>
#include "uart.h"
#include "dataParsing.h"
#include "dataParsing.h"
#include "clock.h"
#include "lcd.h"
#include "common.h"


// Outputs definitions
#define RELAY_INIT 		DDRC |= (1<<PC0)
#define RELAY_TOGGLE	PORTC ^= (1<<PC0)
#define RELAY_ON    	PORTC &= ~(1<<PC0)
#define RELAY_OFF   	PORTC |= (1<<PC0)

#define ALARM_INIT		 DDRC |= (1<<PC1)	// TODO change it in main from OUTPUT_PIN_A111_INIT;
#define ALARM_TOGGLE	PORTC ^= (1<<PC1)
#define ALARM_ON    	PORTC &= ~(1<<PC1)
#define ALARM_OFF   	PORTC |= (1<<PC1)


uint8_t setTimerFlag;
uint8_t executedFlag;
uint8_t alarmActive;
volatile uint8_t enbleSrvceMode;

uint8_t lockMainScreen;


void turnAlarmOff(void);
void turnAlarmOn(void);
void relOFF(void);
void relON(void);
void outOFF(void);
void outON(void);
void outAlarm(void);

void incrMinMoist(void);

/** @brief  send current time via
 *  @param	None
 */
void uartWriteCurrTime(time currTime);

/** @brief  send temperature via
 *  @param
 *  @param
 */
void uartWriteTemp(TEMP * temperature);

/** @brief  send light percentage via UART
 *  @param  Light as percentage of maximum reading from sensor
 */
void uartWriteLight(uint8_t percent);

/** @brief  send moisture readings from all sensors via UART
 *  @param	Amount of sensors
 *  @param	Array of integers with readings from sensor
 *  		ordered following sensor ID
 */
void uartWriteMoisture(uint8_t sensorCount, uint8_t *percent);

/** @brief  send waterflow and it's volume since pump ON via UART
 *  @param	Waterflow per minute [cl/min]
 *  @param 	Waterflow volume since pump working [cl]
 */
void uartWriteWaterflow(uint32_t perMinute, uint32_t accumulated);

/** @brief	send data from all sensors + time via UART
 *  @param  : temperature	: temperature struct
 *  @param  : light			: Light as percentage of maximum reading from sensor
 *  @param	: moistSenCount	: Amount of sensors
 *  @param	: moistPercent	: Array of integers with readings from moisture sensor
 *  		ordered following sensor ID
 *  @param	Waterflow per minute [cl/min]
 *  @param 	Waterflow volume since pump working [cl] */

void uartWriteSensorData(TEMP *temperature, uint8_t light, uint8_t moistSenCount, uint8_t *moistPercent, uint32_t perMinute, uint32_t accumulated, time _globalTime);

/** @brief	Print text, update global time and clear flags menu and RTC flags
 *  @param  *row1	: buffer with text coded in ASCII to be displayed at first row of LCD
 *  @param  *row2	: buffer with text coded in ASCII to be displayed at second row of LCD
 *  @return	None
 */
void menuItem(char *row1, char *row2, time *_time);

/** @brief	Print text, update global time and clear flags menu and RTC flags
 *  @param  None
 *  @return	None
 */
void serviceModeEntry(void);



void relToggle(void);
void alarmToggle(void);

/**
 * @brief: proceed to normal operation
 */
void exitServiceMode(void);

/** @brief  Write to HD44780 data half byte by half byte while in 4 bit interface mode
 *  @param  older half of byte to send
 */
void incrDigit(void);

#endif
