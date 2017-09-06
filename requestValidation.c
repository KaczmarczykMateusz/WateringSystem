/*
 ============================================================================
 Name        : requestValidation.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description : Decision taking functions for inteligent watering system project
 ============================================================================
 */
#include "requestValidation.h"
/*
void conditionalSwitch(uint32_t turnOnTime, uint32_t activeTime, uint32_t currentTimeAsSeconds, uint8_t moistFlag, float temp,
		void * action1(void), void * action2(void), float waterVolume, uint8_t bright) {
	uint8_t takeActionFlag = 0;
	uint8_t takeActionFlagValidated = 0;

//convert and check time block
	uint32_t onePercentActiveTime = activeTime / 100;
	uint32_t passedTime;

	if(currentTimeAsSeconds > turnOnTime){
		passedTime = currentTimeAsSeconds - turnOnTime;
	} else {
		uint32_t totalFirstDay = ((24*60)*60) - turnOnTime; // entire day - time on = time on during first day
		uint32_t totalSecondDay = ((24*60)*60) - currentTimeAsSeconds;
		passedTime = totalFirstDay + totalSecondDay;
	}
	if(passedTime > (onePercentActiveTime * 80)) {
		takeActionFlag = 1;
	}

// check teperature block
	if(!tempSavedFlag) { //TODO:declare flag and value globally
		savedTemperature = temp;
		tempSavedFlag = 1;
	}
	if(temp > (savedTemperature+1)) { //in case if temperature increased - request system on
		takeActionFlag = 1;
	}

//check moisture sensor
	if(takeActionFlag && moistFlag) {
		takeActionFlagValidated = 1;
	}

// execute action
	if(!actionExecuting){
		if(takeActionFlagValidated) {
		action1();
		actionExecuting = 1;
		}
	} else if(waterVolume > presetWaterVolume) {
		action2();
		actionExecuting = 0;
	}
}
*/
