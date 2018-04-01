/*
 ============================================================================
 Name		: requestValidation.h
 Author		: Mateusz Kaczmarczyk
 Version	:
 Description: Decision-making functions for smart watering system
 ============================================================================
 */
#ifndef REQUEST_VALIDATION_H
#define REQUEST_VALIDATION_H

#include <stdio.h>
#include <avr/io.h>
#include "lcd.h"
#include "common.h"

typedef enum _startAction {
	WAIT			=	(1UL << 0),	//@brief: Wait until next day
	ENABLE			=	(1UL << 1),	//@brief: Wait until time reaches set by user turn ON value
	TURN_ON_TIME	=	(1UL << 2),	//@brief: Wait until increase of temperature or exceeding 70% of set by user time window
	WAIT_TO_CONFIRM	=	(1UL << 3),	//@brief: Wait for confirmation from moisture sensor
	EXECUTE			=	(1UL << 4),	//@brief: Execute action
	IN_PROGRESS		=	(1UL << 5),	//@brief: Wait for condition terminating action
	TERMINATE		=	(1UL << 6)	//@brief: Terminates action
//	START_COUNTER	=	TURN_ON_TIME | WAIT_TO_CONFIRM	//@brief: Starts passed time counter
}processValidation;

/**	@brief:	Assign values from sensors to storage variables
 *	@param:	*_value	: Structure holding all real sensor values
 *	@param: moisture: actual moisture as percents
 *	@param: temp	: actual temperature multiplied by 100
 *	@param: wfVolume: actual water flow volume measured from
 *					  last starting of the pump as centilitres (1cl = 1/100l)
 *	@param: bright	:  actual brightness as percents
 */
value updateSensorValues(uint8_t moisture, uint32_t temp, uint16_t wfVolume, uint8_t bright);

/**
 *
 * 	@brief:	Updates values which are for exclusive usage of conditional switch to determine whether and when
 *			to start/ end action (default: watering)
 * @param:	*_condSwitch: Structure holding all condition values for conditional switch
 * @param:	turnOnTime	: Time (converted to seconds) when to start validation of function execution
 * @param:	activeTime	: Time window (converted to seconds) to perform validation
 * @param:	moistureMin	: Minimum moisture to enable start of watering process passed as percent
 * @param:	presetWf	: Water to be used for one watering as centilitres (1cl = 1/100l)
 * @note:	resolution	: Minutes
 *						  Ensure to call this function after every change of conditions
 *  @see:	conditionalSwitch()
*/
condSwitch updateConditionalSwitch(uint32_t turnOnTime, uint32_t complexCheckTime, uint8_t moistureMin, uint16_t presetWf, uint32_t procesTime);

/**
 * @brief:	Prototype of starting action callback function to be replaced with executing function
 * @see:	used in conditionalSwitch()*/
void (*startCallback)(void);

/**
 * @brief:	Prototype of ending action callback function to be replaced with executing function
 * @see:	used in conditionalSwitch()*/
void (*endCallback)(void);

/**
 * @brief:	Registers callback to function triggered by conditional switch
 * 			(by default: water pump relay ON)
 * @param:	Pointer to function with no parameters neither return
 */
void registerStartActionCallback(void (*startAction)(void));

/**
 * @brief: Registers callback to function triggered by conditional switch
 * 		    by default: water pump relay OFF)
 * @param: Pointer to function with no parameters neither return
 */
void registerEndActionCallback(void (*endAction)(void));

/**
 * @brief: Main function for starting and terminating action
 * 		    (by default watering) checking whether all conditions occurred
 * @param: Current global time as seconds
 * @param: Flag terminating action before it expires
 */
status conditionalSwitch(condSwitch _condSwitch, value _value, uint32_t currentTime, uint8_t activate);

/**
 * @brief:	Feature enables user to set his own timer
 *			where he specifies turning ON and OFF time and actions
 *			triggered at indicated time
 * @param:	currentTime		: Current time as seconds
 * @param:	turnOnTime		: Time when to start executing action as seconds
 * @param:	activeTime		: Time to keep execution of "alarm" task ON as seconds
 * @return:	determine system state (work/ready)
 */
uint8_t timerSwitch(uint32_t currentTime, uint32_t turnOnTime, uint32_t activeTime);

#endif
