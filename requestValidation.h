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

//@brief: Holds conditional switch condition variables
typedef struct _condSwitch {
	uint8_t moistureMin;	//@brief: Minimum moisture at which watering process can be triggered
	uint16_t presetWf ;		//@brief: Water volume which is to be applied during watering process
	uint32_t turnOnTime;	//@brief: Time at which further validation enabled
	uint32_t activeTime;	//@brief: Time window in which system process validation
}condSwitch;

//@brief: Holds actual values from sensors
typedef struct _value {
	uint8_t moisture;	//@brief: Actual moisture as percents
	uint8_t bright;		//@brief: Actual brightness as percents
	uint16_t wfVolume;	//@brief: Actual water flow volume as centilitres (1cl = 1/100l)
	uint32_t temp;		//@brief: Actual temperature multiplied by 100
}value;


/**	@brief:	Assign values from sensors to storage variables
 *	@param:	*_value	: Structure holding all real sensor values
 *	@param: moisture: actual moisture as percents
 *	@param: temp	: actual temperature multiplied by 100
 *	@param: wfVolume: actual water flow volume measured from
 *					  last starting of the pump as centilitres (1cl = 1/100l)
 *	@param: bright	:  actual brightness as percents
 */
void updateSensorValues(value (*), uint8_t, uint32_t, uint16_t, uint8_t);

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
void updateConditionalSwitch(condSwitch (*),uint32_t turnOnTime, uint32_t activeTime, uint8_t moistureMin, uint16_t presetWf);

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
void conditionalSwitch(condSwitch *_condSwitch, value * _value, uint32_t currentTime, uint8_t *shutDown);

#endif
