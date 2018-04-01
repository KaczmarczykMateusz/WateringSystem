 /*
 ============================================================================
 Name        :	spi.h
 Author      :	Mateusz Kaczmarczyk
 Version     :	Atmega32, should work at Atmega8 too
 Description :	See uart.h
 ============================================================================
 */
#ifndef __COMMON_H
#define __COMMON_H

typedef enum {
	MINUTES,
	LITRES
} control;

 typedef enum {
 	NOT_READY			=	(1UL << 0),	//@brief: Won't seek for executing action
 	READY				=	(1UL << 1),	//@brief: Wait until time reaches set by user turn ON value
 	CHECK_TIME_AND_TEMP	=	(1UL << 2),	//@brief: Wait until increase of temperature or exceeding 70% of set by user time window
 	CHECK_MOIST			=	(1UL << 3),	//@brief: Wait for confirmation from moisture sensor
 	WORK				=	(1UL << 4),	//@brief: Wait for the terminating condition
 } status;

 typedef enum {
 	MOIST_ON,
 	MOIST_OFF
 } moistureCtrl;

 //@brief: Holds actual values from sensors
 typedef struct {
 	uint8_t moisture;	//@brief: Actual moisture as percents
 	uint8_t bright;		//@brief: Actual brightness as percents
 	uint16_t wfVolume;	//@brief: Actual water flow volume as centilitres (1cl = 1/100l)
 	uint32_t temp;		//@brief: Actual temperature multiplied by 100
 } value;

 //@brief: Holds operation mode
 typedef enum {
 	SIMPLE,		//@brief: Take action at time
 	HUMIDITY,	//@brief: Take action at time if moisture lower than minimum
 	COMPLEX		//@brief: As above plus additional checks
 } complexity;


 //@brief: Holds conditional switch condition variables
 typedef struct _condSwitch {
 	//@brief: Minimum moisture at which watering process can be triggered
 	uint8_t moistureMin;
 	//@brief: Water volume which is to be applied during watering process in case if water volume control
 	uint16_t presetWf ;
 	//@brief: Time at which further validation READYd
 	uint32_t turnOnTime;
 	//@brief: Time window in which system process validation
 	uint32_t complexCheckTime;
 	//@brief: Time how long to execute workStatus in case if time (minutes) control
 	uint32_t procesTime;
 	//@brief: Holds value for complexity of watering system validation check
 	complexity complexMode;
 	//@brief: Determines terminating of workStatus by; litres/ minutes
 	control ctrlMode;
 }condSwitch;

#endif
