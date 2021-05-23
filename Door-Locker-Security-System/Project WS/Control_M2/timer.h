/*----------------------------------------------------------------------------------------------
 *  Module: Timer
 * File Name: timer.
 * AUTHOR: Bassnat Yasser
 * Data Created: 28 / 3 / 2021
 * Description: Header file for the Timer AVR driver
 ------------------------------------------------------------------------------------------------*/

#ifndef TIMER_H_
#define TIMER_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"

typedef enum
{
	NORMAL, COMPARE, COMPAREA, COMPAREB
}Timer_Mode;

typedef enum
{
	Timer0, Timer1, Timer2
}Timer_Number;

typedef enum
{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_32,F_CPU_64,F_CPU_256,F_CPU_1024
}Timer_Clock;

typedef struct
{
	Timer_Mode mode;
	uint16 initial_value;
	Timer_Clock clock;
	uint16 compare_value;
	Timer_Number timer_number;
}Timer_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void Timer_init(const Timer_ConfigType * Config_Ptr);
void Timer_setCallBack(void(*a_ptr)(void));
void Timer_stop(uint8 timer_number);

#endif /* TIMER_H_ */
