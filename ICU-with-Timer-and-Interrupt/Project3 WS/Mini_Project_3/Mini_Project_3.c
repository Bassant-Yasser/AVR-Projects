/*----------------------------------------------------------------------------------------------
 * File Name: Mini_Project_3.c
 *
 * AUTHOR: Bassnat Yasser
 *
 * Data Created: 28 / 3 / 2021
 *
 * Description: Implement EX1 in ICU using Timer1 Overflow mode + External Interrupt (INT0).
 ------------------------------------------------------------------------------------------------*/

#define F_CPU 8000000UL
#include "lcd.h"
#include "timer.h"

#define RISING 0
#define FALLING 1

uint8 g_edgeCount = 0;
uint16 g_timeHigh = 0;
uint16 g_timePeriod = 0;
uint16 g_timePeriodPlusHigh = 0;

uint16 Timer1_getValue()
{
	return TCNT1;
}


void Timer1_clearTimerValue()
{
	TCNT1 = 0;
}

/* External INT0 enable and configuration function */
void INT0_Init(void)
{
	/* Disable interrupts by clearing I-bit */
	SREG  &= ~(1<<7);

	/* Configure INT0/PD2 as input pin */
	DDRD  &= (~(1<<PD2));

	/* Enable external interrupt pin INT0 */
	GICR  |= (1<<INT0);

	/* Trigger INT0 with the raising edge */
	MCUCR |= (1<<ISC00) | (1<<ISC01);

	/* Enable interrupts by setting I-bit */
	SREG  |= (1<<7);
}

void INT0_deinit()
{
	/* Disable external interrupt pin INT0 */
	GICR  &= ~(1<<INT0);
}

void Int0_setEdgeDetectionType(uint8 edge)
{
	if(edge == FALLING)
	{
		MCUCR |= (1<<ISC01);
		MCUCR &= ~(1<<ISC00);
	}
	else if(edge == RISING)
	{
		MCUCR |= (1<<ISC01);
		MCUCR |= (1<<ISC00);
	}
}


ISR(INT0_vect)
{
	g_edgeCount++;
	if(g_edgeCount == 1)
	{
		/*
		 * Clear the timer counter register to start measurements from the
		 * first detected rising edge
		 */
		Timer1_clearTimerValue();
		/* Detect falling edge */
		Int0_setEdgeDetectionType(FALLING);
	}
	else if(g_edgeCount == 2)
	{
		/* Store the High time value */
		g_timeHigh = Timer1_getValue();

		/* Detect rising edge */
		Int0_setEdgeDetectionType(RISING);
	}
	else if(g_edgeCount == 3)
	{
		/* Store the Period time value */
		g_timePeriod = Timer1_getValue();

		/* Detect falling edge */
		Int0_setEdgeDetectionType(FALLING);
	}
	else if(g_edgeCount == 4)
	{
		/* Store the Period time value + High time value */
		g_timePeriodPlusHigh = Timer1_getValue();

		/* Clear the timer counter register to start measurements again */

		/* Detect rising edge */
		Int0_setEdgeDetectionType(RISING);
	}
}

int main()
{
	uint32 dutyCycle = 0;

	/* Initialize both the LCD driver */
	LCD_init();

	/* Create configuration structure for Timer driver */
	Timer_ConfigType Config_Ptr = {NORMAL, 0, F_CPU_CLOCK, 0, Timer1};

	/* Initialize timer */
	Timer_init(&Config_Ptr);

	/* Initialize INT0 */
	INT0_Init();

	/* Enable Global Interrupt I-Bit */
	SREG |= (1<<7);



	while(1)
	{
		if(g_edgeCount == 4)
		{
			/* Disable INT0 */
			INT0_deinit();

			g_edgeCount = 0;
			LCD_displayString("Duty = ");

			/* calculate the dutyCycle */
			dutyCycle = ((float)(g_timePeriodPlusHigh-g_timePeriod) / (g_timePeriodPlusHigh - g_timeHigh)) * 100;

			/* display the dutyCycle on LCD screen */
			LCD_intgerToString(dutyCycle);
			LCD_displayCharacter('%');
		}
	}
}
