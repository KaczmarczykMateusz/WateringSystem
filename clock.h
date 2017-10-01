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

#define SEC_CHANGED_SET		timeFlag |= (1 << 0)
#define SEC_CHANGED_CHECK	timeFlag &= (1 << 0)
#define SEC_CHANGED_CLEAR	timeFlag &= ~(1 << 0)

#define MIN_CHANGED_SET		timeFlag |= (1 << 1)
#define MIN_CHANGED_CHECK	timeFlag &= (1 << 1)
#define MIN_CHANGED_CLEAR	timeFlag &= ~(1 << 1)

#define DAY_CHANGED_SET		timeFlag |= (1 << 2)
#define DAY_CHANGED_CHECK	timeFlag &= (1 << 2)
#define DAY_CHANGED_CLEAR	timeFlag &= ~(1 << 2)

#include "keyboard.h"

//  @brief  Flag indicating time changed in ISR
volatile uint8_t timeFlag;
//  @brief  Value normally should be changed only inside ISR
volatile uint8_t second;

//  @brief  Structure holding date and time
typedef struct {
	volatile uint8_t second;
	volatile uint8_t minute;
	uint8_t hour;
	uint8_t day;
} time;

/** @brief  Initialisation of Real Time Clock
 * based on 32 768 kHz cristal oscillator connected to TOSC1 and TOSC2 Atmega PINs
 * using Atmega Timer2
 *  @param  None
 *  @return None
 */
void rt_clock_init(void);

/** @brief  Setting time as 24h format by user
 *  @param  Struct consisting of button parameters
 *  @param  Struct consisting of time to be set
 *  @return Set if time got changed, and modify time values
 */
uint8_t setTime(tButton * btn, time *tmp);

/** @brief  Time transforming from seconds into universal format
 * 	adding seconds into minutes etc.
 *  @param  Struct consisting of days, hours and seconds //TODO: add weeks and years
 *  @return None but modify of all struct values
 */
void timeDivision(time *tmp);

/** @brief  Transform 24h format time into seconds
 *  @param  Struct consisting of hours and seconds
 *  @return None but modify both values
 */
uint32_t timeToSeconds(time *tmp);

/** @brief  Feature enables user to set his own timer
 *  where he specifies turning ON and OFF time and actions
 *  triggered at indicated time
 *
 *  @param  Turning ON time as seconds
 *  @param  Turning OFF time as seconds
 *  @param  Action triggered at first (turning ON) indicated time
 *  @param  Action triggered at second (turning OFF) indicated time
 *  @return none
 */
void userTimer(uint32_t turnOnTime, uint32_t turnOffTime, void (*actionON)(void), void (*actionOFF)(void), uint32_t currentTime);

/** @brief  Gets time from struct indicated by user and returns it as seconds
 *  @param  Struct consisting of hours minutes and seconds
 *  @return Time counted from passed Struct as second
 */
uint32_t getCurrentTime(time *tmp);

/** @brief  Initialisation of power save mode without entering it.
 * It is possible to enter power saving/ sleep mode with
 * another function (goToSleep)
 *  @param  None
 *  @return None
 */
void powerSaveInit(void);

/** @brief  Putting microcontroler into sleep/ power saving mode
 *  @param  None
 *  @return None
 */
void goToSleep(void);

#endif
