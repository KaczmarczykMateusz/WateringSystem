/*
 ============================================================================
 Name        : moistureSensor.c
 Author      : Mateusz Kaczmarczyk
 Version     :
 Description :
 ============================================================================
 */
#include "moistureSensor.h"

//check values for calibration
void checkMoistSensor(void) {
	sprintf(bufferLCD, "Moist calibrate");
	LCD_Clear();
	LCD_GoTo(0,0);
	LCD_WriteText(bufferLCD);

	float currentMoist = (float)adcOversampleEfficient();
	//convert result to percent increasing (flip ADC result)
	if(currentMoist > 0) {
		currentMoist *= 0.012220703125;
		currentMoist *= (-1);
		currentMoist += 100;
	} else {
		currentMoist = 0;
	}
	sprintf(bufferLCD, "Current val: %2.1f", (double)currentMoist);
	LCD_GoTo(0,1);
	LCD_WriteText(bufferLCD);
	_delay_ms(1000);

	sprintf(bufferLCD, "Continue: 'sel'");
	LCD_Clear();
	LCD_GoTo(0,0);
	LCD_WriteText(bufferLCD);
	_delay_ms(1000);

}

void calibrateMoistSensor(tButton * setBtn, tButton * selectBtn) {
	key_press(selectBtn, choiceOfMenu, NULL);
	if(choseMenu == 0) {
		sprintf(bufferLCD, "enter min moist: %d", minMoist);
		LCD_Clear();
		LCD_GoTo(0,0);
		LCD_WriteText(bufferLCD);
		key_press(setBtn, incrementMinVal, NULL);
	} else if(choseMenu == 1) {
		sprintf(bufferLCD, "enter max moist: %d", maxMoist);
		LCD_GoTo(0,1);
		LCD_WriteText(bufferLCD);
		key_press(setBtn, incrementMaxVal, NULL);
	}
}

// Moisture sensor works only with 5V reference voltage (it needs minimum 3.7V)
// Max moist is used to calibrate sensor, if set to zero put sensor to water and read value which you save to this variable
float measureMoisture(void) {
	maxMoist = 70;

	if(maxMoist) {
		uint16_t currentMoist = (double)adcOversampleEfficient();
		int8_t moistureAsPercent;

		//convert result to percent increasing (flip ADC result)
		if(currentMoist > 0) {
			float maxMoistTemp = (float)(float)maxMoist/100 * 8192;

			uint16_t scope = 8192 - (8192 - maxMoistTemp);
			float scopePercent = (float)100 / scope;

			currentMoist -= 8192 - maxMoistTemp;
			moistureAsPercent = (currentMoist * scopePercent);
			moistureAsPercent *= (-1);
			moistureAsPercent += 100;

		} else {
			moistureAsPercent = 0;
		}
		return moistureAsPercent; // Error
	}
	return 199; // Error
}

uint8_t writeMoisture(volatile uint8_t * KPIN) {
	MOISR_SENSORS_INIT;
	uint8_t enableSystemOn = 0;
	for(uint8_t i = 0; i<6; i++) {
		*KPIN |= (1<<i); // TODO: check if without delay sensor is working properly (fast enough)
		moistSensors[i] = measureMoisture();
		*KPIN &= ~(1<<i);
	}
	return enableSystemOn;
}

void moisturePrintOnLCD(void) {
	uint8_t i;
	for(i=0; i<7; i+=2) {
		if(i < 6) {
			sprintf(bufferLCD, "Sensor %d: :%d%%", i, moistSensors[i]);
			LCD_Clear();
			LCD_GoTo(0,0);
			LCD_WriteText(bufferLCD);

			sprintf(bufferLCD, "Sensor %d: :%d%%", i, moistSensors[i]);
			LCD_GoTo(0,1);
			LCD_WriteText(bufferLCD);

			_delay_ms(1000);
		}
	}
}

void incrementMinVal(void) {
	if(minMoist<=100) {
	 ++minMoist;
	} else {
	 minMoist = 0;
	}
}

void incrementMaxVal(void) {
	if(maxMoist<=100) {
	 ++maxMoist;
	} else {
	 maxMoist = 0;
	}
}

void choiceOfMenu(void) {
	if(choseMenu < 1) {
		++choseMenu;
	} else {
		choseMenu = 0;
	}
}
