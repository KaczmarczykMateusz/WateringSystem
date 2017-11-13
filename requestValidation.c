/*
 ============================================================================
 Name        : requestValidation.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description : Decision taking functions for smart watering system project
 ============================================================================
 */
#include <stdio.h>
#include "requestValidation.h"
#include "main.h"

/*************************************************************************
 Function: updateConditionalSwitch()
 Purpose:
 Input:    turnOnTime	: Time (converted to seconds) when to start validation of function execution
		   activeTime	: Time window (converted to seconds) to perform validation
		   moistureMin	: Minimum moisture to enable start of watering process passed as percent
		   presetWf		: Water to be used for one watering as whole litres
 notice resolution: minutes
 	 	 	 	 	ensure to call this function after every change of conditions
 **************************************************************************/
void updateConditionalSwitch(uint32_t turnOnTime, uint32_t activeTime, uint8_t moistureMin, uint16_t presetWf) {
	switchConditions.turnOnTime = turnOnTime;
	switchConditions.activeTime = activeTime;
	switchConditions.moistureMin = moistureMin;
	switchConditions.presetWf = presetWf;
}


/*************************************************************************
 Function: conditionalSwitch()
 Purpose:
 Input:    moisture		: Moisture of soil as percents
		   temp			: Temperature multiplied by 100
		   wfVolume		: Water volume which was used for watering till now as whole litres
		   bright		: Brightness of environment in order to indicate day/ night
 notice resolution: minutes
 **************************************************************************/
void updateSensorValues(uint8_t moisture, uint32_t temp, uint16_t wfVolume, uint8_t bright) {
	val.moisture = moisture;
	val.temp = temp;
	val.wfVolume = wfVolume;
	val.bright = bright;
}


/*************************************************************************
 Function: 	registerStartActionCallback()
 Purpose:	Registers callback to function  triggered by conditional switch
 Input:		Void function without return eg. water pump relay ON
 See:		conditionalSwitch()
 **************************************************************************/
void registerStartActionCallback(void (*startAction)(void)) {
	startCallback = startAction;
}

/*************************************************************************
 Function: 	registerStartActionCallback()
 Purpose:	Registers callback to function triggered by conditional switch
 Input:		Void function without return eg. water pump relay ON
 See:		conditionalSwitch()
 **************************************************************************/
void registerEndActionCallback(void (*endAction)(void)) {
	endCallback = endAction;
}

/*************************************************************************
 Function: conditionalSwitch()
 Purpose:
 Input:    currentTime 	: Current time converted to seconds
 notice resolution: minutes
 **************************************************************************/
void conditionalSwitch(uint32_t currentTime, uint8_t enable) {

	static enum startAction enableAction = DISABLED;
	static uint32_t savedTemperature = 0;
	uint32_t passedTime = 0;
	if(!enable) {
		enableAction = DISABLED;
		enable = 1;
	}
	if(enableAction != DISABLED) {
		if(currentTime >= switchConditions.turnOnTime){
			passedTime = currentTime - switchConditions.turnOnTime;

		} else {
			//change code below (remove additional variables) if sure that equations are correct
			uint32_t totalFirstDay = (uint32_t)(((uint32_t)24*60)*60) - switchConditions.turnOnTime; // entire day - time on = time on during first day
			uint32_t totalSecondDay = (uint32_t)(((uint32_t)24*60)*60) - currentTime;
			passedTime = totalFirstDay + totalSecondDay;
		}
	}

	switch(enableAction) {
	case DISABLED:
		if(currentTime >= switchConditions.turnOnTime){
			savedTemperature = val.temp;
			enableAction = START_CHECK;
		}
		LCD_GoTo(10, 1);
		LCD_WriteText("-0");
	break;
	case START_CHECK:
		if(passedTime == (switchConditions.activeTime/2)) { // TODO disable while zero
			enableAction = WAIT_TO_VALIDATE;
		}
		LCD_GoTo(10, 1);
		LCD_WriteText("-1");
#if 0 //enable instead of previous after ensuring that it won't make issues and other working properly
		if(passedTime > ((condSwitch.activeTime) * 0.8f)) {
			enableAction = WAIT_TO_VALIDATE;
		}
#endif
#if 0 //Enable after first part of condition works properly
		else if(temp > (savedTemperature+100)) {			//in case if temperature increased by 1 Celsius - request system on
			enableAction = WAIT_TO_VALIDATE;
		}
#endif
	break;
	case WAIT_TO_VALIDATE:
		if(val.moisture < switchConditions.moistureMin) {	//check moisture sensor
			enableAction = CONFIRM_VALIDATION;
		}
		LCD_GoTo(10, 1);
		LCD_WriteText("-2");
	break;
	case CONFIRM_VALIDATION:
		if(startCallback) {
			startCallback();					// execute action
		}
		enableAction = ACTION_STARTED;
		LCD_GoTo(10, 1);
		LCD_WriteText("-3");
	break;

	case ACTION_STARTED:
#if 0
	else if(val.wfVolume > condSwitch.presetWf &&
			enableAction == ACTION_STARTED) {
		if(endCallback) {
			endCallback();
		}
		enableAction = DISABLED;
		actionExecuting = 0; TODO: activate when waterflow sensor connected
	}
#endif
		LCD_GoTo(10, 1);
		LCD_WriteText("-4");
	break;

	default:
		enableAction = DISABLED;
	}
}
