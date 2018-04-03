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
condSwitch updateConditionalSwitch(uint32_t turnOnTime, uint32_t complexCheckTime, uint8_t moistureMin, uint16_t presetWf, uint32_t procesTime) {
	condSwitch _condSwitch;
	_condSwitch.turnOnTime = turnOnTime;
	_condSwitch.complexCheckTime = complexCheckTime;
	_condSwitch.moistureMin = moistureMin;
	_condSwitch.presetWf = presetWf;
	_condSwitch.procesTime = procesTime;
	return _condSwitch;
}

/*************************************************************************
 Function: 	conditionalSwitch()
 Purpose:	Check when to start/ stop defined currentStatus (by default: watering)
 Input:    	CurrentTime 	: Current time converted to seconds
 See: 		resolution: seconds
 **************************************************************************/
status conditionalSwitch(condSwitch _condSwitch, value _value, uint32_t currentTime, uint8_t activate) {
	if(!startCallback || !endCallback) {
		return NOT_READY;
	}

	static status currentStatus = NOT_READY;
	currentStatus = activateSwitch(activate, currentStatus);

	switch(currentStatus) {

	case NOT_READY:	// purpose: don't perform check before switch activated
	break;

	case READY:	// purpose: Wait until time reaches set by user turn ON value
		if(currentTime >= _condSwitch.turnOnTime) {
			currentStatus = checkComplexity(_condSwitch);
		}
	break;

	case CHECK_TIME_AND_TEMP:	// purpose: Wait until increase of temperature or exceeding 70% of set by user time window
		if(timeTempSwitch(_condSwitch, currentTime, _value.temp)) {
			currentStatus = CHECK_MOIST;
		}
	break;

	case CHECK_MOIST:	// purpose: Check moisture sensor
		if(_value.moisture < _condSwitch.moistureMin) {
			startCallback();
			currentStatus = WORK;
		}
	break;

	case WORK:	// purpose: Wait till the end of currentStatus
		if(_condSwitch.ctrlMode == LITRES) {
			if(_value.wfVolume > _condSwitch.presetWf) {
				endCallback();
				currentStatus = NOT_READY;
			}
		} else {
			currentStatus =  timer(_condSwitch, currentTime, currentStatus);
		}
	break;

	default:
		endCallback();
		currentStatus = NOT_READY;
	}
	return currentStatus;
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

	static uint8_t activateHold = 0;
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

	currentStatus = timer(_condSwitch, currentTime, currentStatus);

	return currentStatus;
}

/*************************************************************************
 Function:	timer()
 Purpose:
 Input:
 Notice:
 **************************************************************************/
status timer(condSwitch _condSwitch, uint32_t currentTime, status currentStatus) {
	if(!endCallback) {
		return NOT_READY;
	}

	if(currentTime > (_condSwitch.turnOnTime+_condSwitch.procesTime)) {
		endCallback();
		currentStatus = NOT_READY;
	} else if(currentTime<_condSwitch.turnOnTime) {
		if(  ( (((uint32_t)(24*60)*60)-_condSwitch.turnOnTime)+currentTime ) > (currentTime+_condSwitch.procesTime)  ) {
			endCallback();
			currentStatus = NOT_READY;
		}
	}
	return currentStatus;
}

/*************************************************************************
 Function:	timeTempSwitch()
 Purpose:
 Input:
 Notice:
 **************************************************************************/
uint8_t timeTempSwitch(condSwitch _condSwitch, uint32_t currentTime, uint32_t currentTemp) {
	static uint32_t passedTime = 0;
	static uint32_t savedTemperature = 0;
	if(currentTime == _condSwitch.turnOnTime) {
		savedTemperature =	currentTemp;
	}
	if(_condSwitch.turnOnTime <= currentTime) {
		passedTime = currentTime - _condSwitch.turnOnTime;
	} else {
		passedTime = (  ((uint32_t)(24*60)*60) - _condSwitch.turnOnTime  ) + currentTime;
	}
	if(		(passedTime > ((_condSwitch.complexCheckTime) * 0.7f)) ||	// In case 70% of time passed but temperature didn't increase
			(currentTemp > (savedTemperature + 100)) ) {				// Or in case if temperature increased by 1 Celsius
		return 1;
	} else {
		return 0;
	}
}

/*************************************************************************
 Function:	checkComplexity()
 Purpose:
 Input:
 Notice:
 **************************************************************************/
status checkComplexity(condSwitch _condSwitch) {
	status currentStatus;
	if(_condSwitch.complexMode & COMPLEX) {
		currentStatus = CHECK_TIME_AND_TEMP;
	} else if(_condSwitch.complexMode & HUMIDITY) {
		currentStatus = CHECK_MOIST;
	} else {
		startCallback();
		currentStatus = WORK;
	}
	return currentStatus;
}
