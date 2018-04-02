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
 Purpose:	Check when to start/ stop defined workStatus (by default: watering)
 Input:    	CurrentTime 	: Current time converted to seconds
 See: 		resolution: seconds
 **************************************************************************/
status conditionalSwitch(condSwitch _condSwitch, value _value, uint32_t currentTime, uint8_t activate) {

	static status workStatus = NOT_READY;
	static uint32_t savedTemperature = 0;
	uint32_t passedTime = 0;

	static uint8_t activateHold = 0;
	if(activateHold != activate) {
		if(endCallback) {
			endCallback();
		}
		if(!activate) {
			workStatus = NOT_READY;
		} else {
			workStatus = READY;
		}
	}
	activateHold = activate;

	/* purpose: Check when to start/ stop defined workStatus (by default: watering) */
	switch(workStatus) {

	/* purpose: prevent form executing workStatus before timer set*/
	case NOT_READY:
	break;

	/* purpose: Wait until time reaches set by user turn ON value */
	case READY:
		if(currentTime >= _condSwitch.turnOnTime){
			if(_condSwitch.complexMode & COMPLEX) {
				savedTemperature = _value.temp;
				workStatus = CHECK_TIME_AND_TEMP;
			} else if(_condSwitch.complexMode & HUMIDITY) {
				workStatus = CHECK_MOIST;
			} else {
				if(startCallback) {
					startCallback();
				}
				workStatus = WORK;
			}
		}
	break;

	/* purpose: Wait until increase of temperature or exceeding 70% of set by user time window */
	case CHECK_TIME_AND_TEMP:
		if(_condSwitch.turnOnTime <= currentTime) {
			passedTime = currentTime - _condSwitch.turnOnTime;
		} else {
			passedTime = (  ((uint32_t)(24*60)*60) - _condSwitch.turnOnTime  ) + currentTime;
		}
		if(		(passedTime > ((_condSwitch.complexCheckTime) * 0.7f)) ||	// In case 70% of time passed but temperature didn't increase
				(_value.temp > (savedTemperature + 100)) ) {				// Or in case if temperature increased by 1 Celsius
			workStatus = CHECK_MOIST;							// Request system on
		}
	break;

	/* purpose: Check moisture sensor */
	case CHECK_MOIST:
		if(_value.moisture < _condSwitch.moistureMin) {
			if(startCallback) {
				startCallback();
			}
			workStatus = WORK;
		}
	break;

	/* purpose: Wait till the end of workStatus */
	case WORK:
		if(_condSwitch.ctrlMode == LITRES) {
			if(_value.wfVolume > _condSwitch.presetWf) {
				if(endCallback) {
					endCallback();
				}
				workStatus = NOT_READY;
			}
		} else {
			if(currentTime > (_condSwitch.turnOnTime+_condSwitch.procesTime)) {
				/* purpose: Terminate workStatus */
				if(endCallback) {
					endCallback();
				}
				workStatus = NOT_READY;
			} else if(currentTime<_condSwitch.turnOnTime) {
				if(  ( (((uint32_t)(24*60)*60)-_condSwitch.turnOnTime)+currentTime ) > (currentTime+_condSwitch.procesTime)  ) {
					/* purpose: Terminate workStatus */
					if(endCallback) {
						endCallback();
					}
					workStatus = NOT_READY;
				}
			}
		}
	break;

	default:
		if(endCallback) {
			endCallback();
		}
		workStatus = NOT_READY;
	}
	return workStatus;
}

/*************************************************************************
 Function:	timerSwitch()
 Purpose:	lets user to set action which should be taken at indicated time
 Input:		Global current time, Time to start, time from start till the end, start and finish
 Notice:	Time resolution: minutes
 **************************************************************************/
uint8_t timerSwitch(uint32_t currentTime, uint32_t turnOnClock, uint32_t activeTime) {
	if(!startCallback || !endCallback || !activeTime) {
		return 0;
	}

	static uint8_t isOn = 0;
	if( (currentTime >= turnOnClock) && !isOn ) {
		startCallback();
		isOn= 1;
	} else if(!isOn) {
		return isOn;
	}

	static uint32_t passedTime = 0;
	if(currentTime >= turnOnClock) {
		passedTime = currentTime - turnOnClock;
	} else {
		passedTime = ( (24*60)-turnOnClock ) + currentTime;
	}

	if(passedTime >+ activeTime) {
		endCallback();
		isOn = 0;
		passedTime = 0;
	}
	return isOn;
}
