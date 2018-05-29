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
#include "action.h"

/*************************************************************************
 Function:	registerStartActionCallback()
 Purpose:	Registers callback to function  triggered by conditional switch
 	 	 	(by default: water pump relay ON)
 Input:		Void function without return
 See:		conditionalSwitch()
 **************************************************************************/
void registerStartActionCallback(void (*startAction)(void)) {
	startCallback = startAction;
}

/*************************************************************************
 Function:	registerEndActionCallback()
 Purpose:	Registers callback to function triggered by conditional switch
 	 	 	(by default: water pump relay OFF)
 Input:		Void function without return
 See:		conditionalSwitch()
 **************************************************************************/
void registerEndActionCallback(void (*endAction)(void)) {
	endCallback = endAction;
}

/*************************************************************************
 Function:	updateSensorValues()
 Purpose:	Updates conditional switch conditions
 Input:	_value		: 	Structure holding all real sensor values
 		moisture	:	Moisture of soil as percents
		temp		:	Temperature multiplied by 100
		wfVolume	:	Water volume which was used for watering as centilitres (1cl = 1/100l)
		bright		:	Brightness of environment in order to indicate day/ night
 See: 	resolution: minutes
 **************************************************************************/
value updateSensorValues(uint8_t moisture, uint32_t temp, uint16_t wfVolume, uint8_t bright) {
	value _value;
	_value.moisture = moisture;
	_value.temp = temp;
	_value.wfVolume = wfVolume;
	_value.bright = bright;
	return _value;
}


/*************************************************************************
 Function: updateConditionalSwitch()
 Purpose:
 Input:	*_condSwitch : Structure holding all condition values for conditional switch
 	 	turnOnTime	: Time (converted to seconds) when to start validation of function execution
		activeTime	: Time window (converted to seconds) to perform validation
		moistureMin	: Minimum moisture to enable start of watering process passed as percent
		presetWf	: Water to be used for one watering as centilitres (1cl = 1/100l)
 Return: Filled cond switch struct
 Notice: resolution: minutes
 	 	 	 	 	ensure to call this function after every change of conditions
 **************************************************************************/
condSwitch updateConditionalSwitch(uint32_t turnOnTime, uint8_t moistureMin, uint16_t presetWf, uint32_t procesTime) {
	condSwitch _condSwitch;
	_condSwitch.turnOnTime = turnOnTime;
	_condSwitch.moistureMin = moistureMin;
	_condSwitch.presetWf = presetWf;
	_condSwitch.procesTime = procesTime;
	return _condSwitch;
}

/*************************************************************************
 Function: 	conditionalSwitch()
 Purpose:	Check when to start/ stop defined newStatus (by default: watering)
 Input:    	CurrentTime 	: Current time converted to seconds
 	 	 	activate: value of SYSTEM_STATUS_FLAGS
 See: 		resolution: seconds
 **************************************************************************/
status conditionalSwitch(condSwitch _condSwitch, value _value, uint32_t currentTime, uint8_t activate) {
	if(!startCallback || !endCallback) {
		return NOT_READY;
	}

	static status newStatus = NOT_READY;
	newStatus = activateSwitch(activate, newStatus);

	switch(newStatus) {

	case NOT_READY:	// purpose: don't perform check before switch activated
	break;

	case READY:	// purpose: Wait until time reaches set by user turn ON value
		if(currentTime >= _condSwitch.turnOnTime) {
			if(!actionExecuted) {
				if(_condSwitch.moistCtrl & MOIST_CHECK_ON) {
					newStatus = CHECK_MOIST;
				} else {
					startCallback();
					newStatus = WORK;
				}
				actionExecuted = 1;
			}
		} else {
			actionExecuted = 0;
		}
	break;

	case CHECK_MOIST:	// purpose: Check moisture sensor
		if(_value.moisture < _condSwitch.moistureMin) {
			startCallback();
			newStatus = WORK;
		}
	break;

	case WORK:	// purpose: Wait till the end of newStatus
		if(_condSwitch.ctrlFactor == LITRES) {
			if(_value.wfVolume >= _condSwitch.presetWf) {
				endCallback();
				newStatus = READY;
			}
		} else {
			newStatus =  terminatingTimer(_condSwitch, currentTime, newStatus);
		}
	break;

	default:
		endCallback();
		newStatus = NOT_READY;
	}
	return newStatus;
}

/*************************************************************************
 Function:	activateSwitch()
 Purpose:
 Input:
 Notice:
 **************************************************************************/
status activateSwitch(uint8_t activate, status currentStatus) {
	if(!endCallback) {
		return NOT_READY;
	}

	static uint8_t activateHold = NOT_READY;
	if(activateHold != activate) {
		endCallback();
		if(!activate) {
			currentStatus = NOT_READY;
		} else {
			currentStatus = READY;
		}
	}
	activateHold = activate;

	return currentStatus;
}

/*************************************************************************
 Function:	timeSwitch()
 Purpose:	Checks time in order to take OFF/ ON action
 Input:		Global current time, Time to start, time from start till the end, start and finish
 Notice:	Time resolution: minutes
 **************************************************************************/
status timeSwitch(condSwitch _condSwitch, uint32_t currentTime, status currentStatus) {
	if(!startCallback || !endCallback || !_condSwitch.procesTime) {
		return currentStatus;
	}

	if( (currentTime >= _condSwitch.turnOnTime) && (currentStatus != WORK) ) {
		startCallback();
		currentStatus = WORK;
	} else if(currentStatus != WORK) {
		return currentStatus;
	}

	currentStatus = terminatingTimer(_condSwitch, currentTime, currentStatus);

	return currentStatus;
}

/*************************************************************************
 Function:	terminatingTimer()
 Purpose:
 Input:
 Notice:
 **************************************************************************/
status terminatingTimer(condSwitch _condSwitch, uint32_t currentTime, status currentStatus) {
	if(!endCallback) {
		return NOT_READY;
	}
	if(currentTime > (_condSwitch.turnOnTime+_condSwitch.procesTime)) {
		endCallback();
		currentStatus = READY;
	} else if(currentTime<_condSwitch.turnOnTime) {
		if(  ( (((uint32_t)(24*60)*60)-_condSwitch.turnOnTime)+currentTime ) > (currentTime+_condSwitch.procesTime)  ) {
			endCallback();
			currentStatus = READY;
		}
	}
	return currentStatus;
}

