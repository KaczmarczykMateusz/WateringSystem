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
 Function:	updateSensorValues()
 Purpose:	Updates conditional switch conditions
 Input:	_value		: 	Structure holding all real sensor values
 		moisture	:	Moisture of soil as percents
		temp		:	Temperature multiplied by 100
		wfVolume	:	Water volume which was used for watering as centilitres (1cl = 1/100l)
		bright		:	Brightness of environment in order to indicate day/ night
 See: 	resolution: minutes
 **************************************************************************/
void updateSensorValues(value * _value, uint8_t moisture, uint32_t temp, uint16_t wfVolume, uint8_t bright) {
	_value->moisture = moisture;
	_value->temp = temp;
	_value->wfVolume = wfVolume;
	_value->bright = bright;
}

/*************************************************************************
 Function: updateConditionalSwitch()
 Purpose:
 Input:	*_condSwitch : Structure holding all condition values for conditional switch
 	 	turnOnTime	: Time (converted to seconds) when to start validation of function execution
		activeTime	: Time window (converted to seconds) to perform validation
		moistureMin	: Minimum moisture to enable start of watering process passed as percent
		presetWf	: Water to be used for one watering as centilitres (1cl = 1/100l)
 Notice: resolution: minutes
 	 	 	 	 	ensure to call this function after every change of conditions
 **************************************************************************/
void updateConditionalSwitch(condSwitch *_condSwitch, uint32_t turnOnTime, uint32_t activeTime, uint8_t moistureMin, uint16_t presetWf) {
	_condSwitch->turnOnTime = turnOnTime;
	_condSwitch->activeTime = activeTime;
	_condSwitch->moistureMin = moistureMin;
	_condSwitch->presetWf = presetWf;
}

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
 Function: 	conditionalSwitch()
 Purpose:	Check when to start/ stop defined action (by default: watering)
 Input:    	CurrentTime 	: Current time converted to seconds
 See: 		resolution: minutes
 **************************************************************************/
void conditionalSwitch(condSwitch *_condSwitch, value * _value, uint32_t currentTime, uint8_t *shutDown) {

	static processValidation action = TERMINATE;
	static uint32_t savedTemperature = 0;
	uint32_t passedTime = 0;
#if 0
	if(*shutDown == 1) {
		action = TERMINATE;
		*shutDown = 0;
	}
#endif
	if((action & TURN_ON_TIME) || (action & WAIT_TO_CONFIRM)) {
		if(currentTime >= _condSwitch->turnOnTime){
			passedTime = currentTime - _condSwitch->turnOnTime;

		} else {
			//change code below (remove additional variables) if sure that equations are correct
			uint32_t totalFirstDay = (uint32_t)(((uint32_t)24*60)*60) - _condSwitch->turnOnTime; // entire day - time on = time on during first day
			uint32_t totalSecondDay = (uint32_t)(((uint32_t)24*60)*60) - currentTime;
			passedTime = totalFirstDay + totalSecondDay;
		}
	}

	/* purpose: Check when to start/ stop defined action (by default: watering) */
	switch(action) {

	/* purpose: Wait until next day */
	case WAIT:
		if(currentTime < _condSwitch->turnOnTime) {
			action = ENABLE;
		}
		LCD_GoTo(10, 1);
		LCD_WriteText("-WAIT");
	break;

	/* purpose: Wait until time reaches set by user turn ON value */
	case ENABLE:
		if(currentTime >= _condSwitch->turnOnTime){
			savedTemperature = _value->temp;
			action = TURN_ON_TIME;
		}
		LCD_GoTo(9, 1);
		LCD_WriteText("-ENABLE");
	break;

	/* purpose: Wait until increase of temperature or exceeding 70% of set by user time window */
	case TURN_ON_TIME:
		if(		(passedTime > ((_condSwitch->activeTime) * 0.7f)) ||	// In case 70% of time passed but temperature didn't increase
				(_value->temp > (savedTemperature + 100)) ) {				// Or in case if temperature increased by 1 Celsius
			action = WAIT_TO_CONFIRM;							// Request system on
		}
		LCD_GoTo(8, 1);
		LCD_WriteText("-ON_TIME");
	break;

	/* purpose: Check moisture sensor */
	case WAIT_TO_CONFIRM:
		if(_value->moisture < _condSwitch->moistureMin) {
			action = EXECUTE;
		}
		LCD_GoTo(9, 1);
		LCD_WriteText("-WAIT TO CNFRM");
	break;

	/* purpose: Execute action */
	case EXECUTE:
		if(startCallback) {
			startCallback();
		}
		action = IN_PROGRESS;

		LCD_GoTo(8, 1);
		LCD_WriteText("-EXECUTE");
	break;

	/* purpose: Wait till the end of action */
	case IN_PROGRESS:
		if(_value->wfVolume > _condSwitch->presetWf) {

			action = TERMINATE;
		}
		LCD_GoTo(5, 1);
		LCD_WriteText("-IN_PROGRESS");
	break;

	/* purpose: Terminate action */
	case TERMINATE:
		if(endCallback) {
			endCallback();
		}

		action = WAIT;
		LCD_GoTo(5, 1);
		LCD_WriteText("-TERMINATE");
	break;

	default:
		action = TERMINATE;
	}
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
