/*
 ============================================================================
 Name        : moistureSensor.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "moistureSensor.h"

/*************************************************************************
 Function: moistureSensor()
 Purpose:  Process entire ADC conversion from the beginning till the end.
 Input:    ADC voltage struct
 Return:	Moisture as percents
 **************************************************************************/
uint8_t moistureSensor(TVOLT * voltage) {
	uint32_t moistResult;
	moistSensMinOut = 200;					//TODO: needs calibration
	moistSensMaxOut = 455;
	moistResult = (int32_t)voltage->adcVoltRaw;
	moistResult -= moistSensMinOut;
	moistSensMaxOut -= moistSensMinOut;

	moistResult = ((1000000/moistSensMaxOut) * moistResult) *  0.0001f;

	moistResult = flipInteger((int8_t)moistResult);

	if(moistResult <= 1) {
		moistResult = 0;
	} else if (moistResult > 99) {
		moistResult = 100;
	}

	return (uint8_t)moistResult;
}

/*************************************************************************
 Function: moistSensPwrUp()
 Purpose:  Power up single sensor
 Input:    Parameters of ADC PIN to be turned ON
 **************************************************************************/
void moistSensPwrUp(volatile uint8_t *S_DDR, volatile uint8_t *S_PORT, uint8_t sensorMask) {
	*S_DDR |= sensorMask;
	*S_PORT |= sensorMask;
}

/*************************************************************************
 Function: moistSensPwrDwn()
 Purpose:  Shut down single sensor
 Input:    Parameters of ADC PIN to be turned OFF
 **************************************************************************/
void moistSensPwrDwn(volatile uint8_t *S_DDR, volatile uint8_t *S_PORT, uint8_t sensorMask) {
	*S_PORT &= ~sensorMask;
}

/*************************************************************************
 Function: moistCheckStart()
 Purpose:  Powers up sensors to prepare them for ADC conversion
 Input:    Number of sensor to be powered
 Returns:  Flag confirming that some of sensors is powered
 **************************************************************************/
uint8_t moistCheckStart(uint8_t sensNo) {
	uint8_t flag;
	moistSensPwrUp(&DDRA, &PORTA, MOIST_SENSOR_POWER_MASK);
	if(sensNo >= MOIST_SENSORS_NUMBER) {
		flag = 0;
	} else {
		flag = 1;
	}
	return flag;
}

/*************************************************************************
 Function: moistCheckResult()
 Purpose:  Reading result of ADC conversion directly in percent, it's
 	 	 	called when it's sure that sensors are stable powered
 Input:    Struct with ADC voltage parameters, number of sensor
 Returns:  Moisture as percent
 **************************************************************************/
uint8_t moistCheckResult(uint8_t isPowered, TVOLT *mSens, uint8_t *sensNo) {
	uint8_t temporaryMoist = 0;
	if(isPowered) {
		uint16_t adcMoist1 = adcOversample(*sensNo, 3);
		voltAdc(adcMoist1, &mSens[*sensNo]);

		temporaryMoist = moistureSensor(&mSens[*sensNo]);
		// Increment until MOIST_SENSORS_NUMBER which as index is one higher than sensor with highest index
		// when value reaches maximum it makes moistCheckStart to disable further checks
		if(*sensNo < MOIST_SENSORS_NUMBER) {
			++*sensNo;
		} else {
			sensNo = 0;
		}
	}
	moistSensPwrDwn(&DDRA, &PORTA, MOIST_SENSOR_POWER_MASK);

	return temporaryMoist;
}



// ******************** SERVICE MODE ***************** //
/*************************************************************************
 Function: calibrateMoistSensor()
 Purpose:
 Input:
 Returns:  None
 **************************************************************************/
uint8_t calibrateMoistSensor(uint8_t enableServiceMode, uint8_t menuIndex, uint8_t actualMoist, tButton *addBtn, tButton *subtrBtn) {
	if(enableServiceMode == (1<<0)) {
		uint16_t adcPercent = 1000 / 500;					//1000 / (MAX ADC voltage)
		static uint8_t temporaryBoundry = 0;
		char bufferLCD[16];
		char buff1[16];

		// ******************** MENU ***************** //
		switch(menuIndex) {

		case 0:
			temporaryBoundry = (moistSensMinOut * adcPercent) / 10;
#if 0
			incrDcr(addBtn, subtrBtn, &temporaryBoundry, 100, NULL);
#endif
			sprintf(buff1, "SET MIN: %d%%", temporaryBoundry);
			sprintf(bufferLCD, "Read: %d%% ->Next", actualMoist);
			menuItem(buff1, bufferLCD);
			moistSensMinOut = (temporaryBoundry * adcPercent) / 10;		// Convert into volts multiplied by 100
		break;

		case 1:
			temporaryBoundry = (moistSensMaxOut * adcPercent) / 10;
#if 0
			incrDcr(addBtn, subtrBtn, &temporaryBoundry, 100, NULL);
#endif
			sprintf(buff1, "SET MAX: %d%%", temporaryBoundry);
			sprintf(bufferLCD, "Read: %d%% ->Next", actualMoist);
			menuItem(buff1, bufferLCD);
			moistSensMaxOut = (temporaryBoundry * adcPercent) / 10;		// Convert into volts multiplied by 100
		break;
		default:
			return 1;
		}
		return 0;
	}
	return 1;
}
