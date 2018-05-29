/*
============================================================================
Name        :	common.h
Author      :	Mateusz Kaczmarczyk
Version     :
Description :
============================================================================
*/
#ifndef __COMMON_H
#define __COMMON_H
#include <stdint.h>


//  @brief  holding values  for ADC oversampling measurement
typedef struct {
	// @brief   set value in the middle (not precise) of measure range which can be pre-stepped down with voltage divider
	uint16_t ref_adc;

	// @brief   set value (volt) of ADC which set during measuring of ref_adc (multiply this value *100)
	uint16_t ref_v;

	// @brief   voltage multiplied by *100
	uint16_t adcVoltRaw;

	// @brief  	digits to print as ASCII char before coma (xx. full volts)
	char beforeComa[3];

	// @brief   digits to print as ASCII char before coma (.xx of volts)
	char afterComa[3];
} TVOLT;

typedef struct {
	uint32_t tempInt;		// @brief Integer part of temperature result
	uint32_t tempFract;		// @brief Fractional part of temperature result
	uint32_t tempMultip;	// @brief temperature multiplied by (*100)
} TEMP;

typedef enum {
	MINUTES,
	LITRES
} control;

//@Brief: SYSTEM_STATUS_FLAGS
typedef enum {
	NOT_READY			=	(1UL << 0),	//@brief: Won't seek for executing action
	READY				=	(1UL << 1),	//@brief: Wait until time reaches set by user turn ON value
	CHECK_MOIST			=	(1UL << 2),	//@brief: Wait for confirmation from moisture sensor
	WORK				=	(1UL << 3),	//@brief: Wait for the terminating condition
} status;

typedef enum {
	MOIST_CHECK_ON,
	MOIST_CHECK_OFF
} moistureCtrl;

//@brief: Holds actual values from sensors
typedef struct {
	uint8_t moisture;	//@brief: Actual moisture as percents
	uint8_t bright;		//@brief: Actual brightness as percents
	uint16_t wfVolume;	//@brief: Actual water flow volume as centilitres (1cl = 1/100l)
	uint32_t temp;		//@brief: Actual temperature multiplied by 100
} value;

/**
 * @brief	Structure holding date and time
 */
typedef struct {
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
} time;

//@brief: Holds conditional switch condition variables
typedef struct _condSwitch {
	//@brief: Minimum moisture at which watering process can be triggered
	uint8_t moistureMin;
	//@brief: Water volume which is to be applied during watering process in case if water volume control
	uint16_t presetWf ;
	//@brief: Time at which further validation READYd
	uint32_t turnOnTime;
	//@brief: Time how long to execute workStatus in case if time (minutes) control
	uint32_t procesTime;
	//@brief: Determines weather moisture check is active
	moistureCtrl moistCtrl;
	//@brief: Determines terminating of workStatus by; litres/ minutes
	control ctrlFactor;
}condSwitch;

#endif
