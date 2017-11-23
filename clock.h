/*
 ============================================================================
 Name        : clock.h
 Author      : Mateusz Kaczmarczyk
 Version     : Microcontroller : Atmel AVR Atmega32
 Description : 	with 32 768 kHz cristal at TOSC1 & TOSC2 (Timer2)
       			resolution 1 second
 				uses Atmega HW Timer2
 ============================================================================
 */

#ifndef CLOCK_H
#define CLOCK_H
#include <avr/sleep.h>
#include <avr/interrupt.h>

/**
 * @brief	Definitions for timeState values
 * 			Indicating time for refreshing MENU
 */
#define INPUT_MODIFIED_SET		timeState |= (1 << 3)
#define INPUT_MODIFIED_CHECK 	timeState & (1 << 3)
#define INPUT_MODIFIED_CLEAR 	timeState &= ~(1 << 3)

/**
 * @brief	Definitions for timeState values
 * 			Indicating changes in global time
 */
#define SEC_CHANGED_SET			timeState |= (1 << 0)
#define SEC_CHANGED_CHECK		timeState & (1 << 0)
#define SEC_CHANGED_CLEAR		timeState &= ~(1 << 0)

#define MIN_CHANGED_SET			timeState |= (1 << 1)
#define MIN_CHANGED_CHECK		timeState & (1 << 1)
#define MIN_CHANGED_CLEAR		timeState &= ~(1 << 1)

#define DAY_CHANGED_SET			timeState |= (1 << 2)
#define DAY_CHANGED_CHECK		timeState & (1 << 2)
#define DAY_CHANGED_CLEAR		timeState &= ~(1 << 2)

/**
 * @brief	Time changeindicator
 */
volatile uint8_t timeState;

/**
 * @brief	Value normally should be changed only inside ISR
 */
volatile uint8_t second;

/**
 * @brief	Structure holding date and time
 */
typedef struct {
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
} time;

/**
 * @brief:	Initialisation of Real Time Clock
 *			based on 32 768 kHz cristal oscillator connected to TOSC1 and TOSC2 Atmega PINs
 *			using Atmega Timer2
 * @param:	None
 * @return:	None
 */
void rt_clock_init(void);


/**
 * @brief:	Time transforming from seconds into universal format
 *			adding seconds into minutes etc.
 * @param:	Struct consisting of days, hours and seconds //TODO: add weeks and years
 * @return:	None but modify of all struct values
 */
void timeDivision(time *tmp);

/**
 * @brief:	Transform 24h format time into seconds
 * 			skipping seconds (resolutionL: minutes)
 * @param:	Struct with time to convert parameters
 * @return:	None but modify both values
 * @notice:	Resolution: minutes
 */
uint32_t timeToSeconds(time *tmp);

/**
 * @brief:	Transform 24h format time into seconds
 *			with highest possible resolution (seconds)
 * @param:	Struct with time to convert parameters
 * @return:	None but modify both values
 * @notice:	Resolution: seconds
 */
uint32_t timeToSecondsAccurate(time *tmp);

/**
 * @brief:	Feature enables user to set his own timer
 *			where he specifies turning ON and OFF time and actions
 *			triggered at indicated time
 * @param:	turnOnTime		: Time when to start executing action as seconds
 * @param:	activeTime		: Time to keep execution of "alarm" task ON as seconds
 * @param:	*actionON(void)	: Action to be executed at turnOnTime
 * @param:	*actionOFF(void): Action to be executed at turnOnTime + activeTime
 * @param:	currentTime		: Current time as seconds
 * @return:	None
 */
void userTimer(uint32_t turnOnTime, uint32_t activeTime, void (*actionON)(void), void (*actionOFF)(void), uint32_t currentTime);

/**
 * @brief  Gets time from struct indicated by user and returns it as seconds
 * @param  Struct consisting of hours minutes and seconds
 * @return Time counted from passed Struct as second
 */
uint32_t getCurrentTime(time *tmp);

/**
 * @brief	Initialisation of power save mode without entering it.
 *			It is possible to enter power saving/ sleep mode with
 *			another function (goToSleep)
 * @param	None
 * @return	None
 */
void powerSaveInit(void);

/**
 * @brief	Putting microcontroler into sleep/ power saving mode
 * @param	None
 * @return	None
 */
void goToSleep(void);

#endif
