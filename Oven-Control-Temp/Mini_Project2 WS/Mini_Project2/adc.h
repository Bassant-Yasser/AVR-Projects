 /******************************************************************************
 *
 * Module: ADC
 *
 * File Name: adc.h
 *
 * Description: header file for the ATmega16 ADC driver
 *
 * Author: Bassant Yasser
 *
 *******************************************************************************/

#ifndef ADC_H_
#define ADC_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"

#define INTERRUPT_ENABLE
#undef INTERRUPT_ENABLE

extern volatile uint16 data;

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	AREF,AVCC, INTERNAL = 4
}ADC_Voltage_Reference;

typedef enum
{
	Division_Factor_2, Division_Factor_4 = 2, Division_Factor_8, Division_Factor_16, Division_Factor_32, Division_Factor_64, Division_Factor_128
}ADC_Clock;

typedef struct
{
	ADC_Voltage_Reference voltage;
	ADC_Clock clock;
}ADC_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for initialize the ADC driver.
 */
void ADC_init(const ADC_ConfigType * Config_Ptr);

/*
 * Description :
 * Function responsible for read analog data from a certain ADC channel
 * and convert it to digital using the ADC driver.
 */
#ifdef INTERRUPT_ENABLE
void ADC_readChannel(uint8 channel_num);
#else
uint16 ADC_readChannel(uint8 channel_num);
#endif

#endif /* ADC_H_ */
