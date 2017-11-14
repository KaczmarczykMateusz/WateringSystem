/*
 ============================================================================
 Name        : action.h
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#ifndef ACTION_H_
#define ACTION_H_

#include "main.h"

uint8_t setTimerFlag;
uint8_t executedFlag;
uint8_t alarmActive;

uint8_t lockMainScreen;


void turnAlarmOff(void);
void turnAlarmOn(void);
void timerSetModeNextStep(void);
void relOFF(void);
void relON(void);
void outOFF(void);
void outON(void);
void outAlarm(void);

void incrMinMoist(void);

/** @brief  send current time via
 *  @param	None
 */
void uartWriteCurrTime(void);

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

void uartWriteSensorData(TEMP *temperature, uint8_t light, uint8_t moistSenCount, uint8_t *moistPercent, uint32_t perMinute, uint32_t accumulated);

/** @brief
 *  @param
 *  @param
 */
void menuItem(uint8_t *executedFlag, time *currentTime, tButton * btn,  char *buff);

/** @brief  Setting time as 24h format by user
 * 	@param  *addBtn	:	Struct consisting of add button parameters or NULL
 *  @param  *subtrBtn:	Struct consisting of subtract button parameters or NULLL
 *  @param  *val	:	Struct consisting of time to be set or NULL
 *  @param	*tmp	:	Variable to be incremented or NULL
 *  @return Set if time got changed, and modify time values
 *  @notice	val and tmp are interchangable
 */
uint8_t incrDcr(tButton *addBtn, tButton *subtrBtn, uint8_t *val, time *tmp);

#endif
