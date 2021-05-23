/*----------------------------------------------------------------------------------------------
 *  * Module: Timer
 * File Name: timer.c
 * AUTHOR: Bassnat Yasser
 * Data Created: 28 / 3 / 2021
 * Description: Source file for the Timer AVR driver
 ------------------------------------------------------------------------------------------------*/

#include "timer.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_Timer0callBackPtr)(void) = NULL_PTR;
static volatile void (*g_Timer1callBackPtr)(void) = NULL_PTR;
static volatile void (*g_Timer2callBackPtr)(void) = NULL_PTR;

uint8 g_timer_number;
/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER0_OVF_vect)
{
	if(g_Timer0callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the overflow is detected */
		(*g_Timer0callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

ISR(TIMER0_COMP_vect)
{
	if(g_Timer0callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the compare value is detected */
		(*g_Timer0callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}


ISR(TIMER1_OVF_vect)
{

	if(g_Timer1callBackPtr != NULL_PTR)
	{

		/* Call the Call Back function in the application after the overflow is detected */
		(*g_Timer1callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

ISR(TIMER1_COMPA_vect)
{
	if(g_Timer1callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the compare value is detected */
		(*g_Timer1callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}


ISR(TIMER1_COMPB_vect)
{
	if(g_Timer1callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the compare value is detected */
		(*g_Timer1callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}


ISR(TIMER2_OVF_vect)
{
	if(g_Timer2callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the overflow is detected */
		(*g_Timer2callBackPtr)(); /* another method to call the function using pointer to function g_Timer2callBackPtr(); */
	}
}

ISR(TIMER2_COMP_vect)
{
	if(g_Timer2callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the compare value is detected */
		(*g_Timer2callBackPtr)(); /* another method to call the function using pointer to function g_Timer2callBackPtr(); */
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/


void Timer_init(const Timer_ConfigType * Config_Ptr)
{
	/* Saves timer_number in global variable */
	g_timer_number = Config_Ptr->timer_number;

	/* Adjust bits of timer0 */
	if(g_timer_number == Timer0)
	{
		/* Non PWM Mode */
		TCCR0 = (1<<FOC0);

		/* Set Timer initial value */
		TCNT0 = Config_Ptr->initial_value;

		if(Config_Ptr->mode == NORMAL)
		{
			/* Enable Timer0 Overflow Interrupt */
			TIMSK |= (1<<TOIE0);
		}
		else if(Config_Ptr->mode == COMPARE)
		{
			/* CTC Mode WGM01=1 & WGM00=0 */
			TCCR0 |= (1<<WGM01);

			/* Set Compare Value */
			OCR0 = Config_Ptr->compare_value;

			/* Enable Timer0 Compare Interrupt */
			TIMSK |= (1<<OCIE0);
		}

		/* adjust clock mode */
		if(Config_Ptr->clock == F_CPU_CLOCK)
		{
			TCCR0 |= (1<<CS00);
		}
		else if(Config_Ptr->clock == F_CPU_8)
		{
			TCCR0 |= (1<<CS01);
		}
		else if(Config_Ptr->clock == F_CPU_64)
		{
			TCCR0 |= (1<<CS00) | (1<<CS01);
		}
		else if(Config_Ptr->clock == F_CPU_256)
		{
			TCCR0 |= (1<<CS02);
		}
		else if(Config_Ptr->clock == F_CPU_1024)
		{
			TCCR0 |= (1<<CS00) | (1<<CS02);
		}
	}
	else if(g_timer_number == Timer1)
	{
		/* Non PWM Mode */
		TCCR1A = (1<<FOC1A) | (1<<FOC1B);

		/* Set Timer initial value */
		TCNT1 = Config_Ptr->initial_value;

		if(Config_Ptr->mode == NORMAL)
		{
			TCCR1B = 0;

			/* Enable Timer1 Overflow Interrupt */
			TIMSK |= (1<<TOIE1);
		}
		else if(Config_Ptr->mode == COMPAREA)
		{
			/*  CTC Mode WGM12=1 WGM13=0 (Mode Number 4) */
			TCCR1B = (1<<WGM12);

			/* Set Compare Value */
			OCR1A = Config_Ptr->compare_value;

			/* Enable Timer1 Compare A Interrupt */
			TIMSK |= (1<<OCIE1A);
		}
		else if(Config_Ptr->mode == COMPAREB)
		{
			/*  CTC Mode WGM12=1 WGM13=1 (Mode Number 12) */
			TCCR1B = (1<<WGM12) | (1<<WGM13);

			/* Set Compare Value */
			OCR1B = Config_Ptr->compare_value;

			/* Enable Timer1 Compare B Interrupt */
			TIMSK |= (1<<OCIE1B);
		}

		/* adjust clock mode */
		if(Config_Ptr->clock == F_CPU_CLOCK)
		{
			TCCR1B |= (1<<CS10);
		}
		else if(Config_Ptr->clock == F_CPU_8)
		{
			TCCR1B |= (1<<CS11);
		}
		else if(Config_Ptr->clock == F_CPU_64)
		{
			TCCR1B |= (1<<CS10) | (1<<CS11);
		}
		else if(Config_Ptr->clock == F_CPU_256)
		{
			TCCR1B |= (1<<CS12);
		}
		else if(Config_Ptr->clock == F_CPU_1024)
		{
			TCCR1B |= (1<<CS10) | (1<<CS12);
		}

	}
	else if(g_timer_number == Timer2)
	{
		/* Non PWM Mode */
		TCCR2 = (1<<FOC2);

		/* Set Timer initial value */
		TCNT2 = Config_Ptr->initial_value;

		if(Config_Ptr->mode == NORMAL)
		{
			/* Enable Timer2 Overflow Interrupt */
			TIMSK |= (1<<TOIE2);
		}
		else if(Config_Ptr->mode == COMPARE)
		{
			/* CTC Mode WGM21=1 & WGM20=0 */
			TCCR2 |= (1<<WGM21);

			/* Set Timer compare value */
			OCR2 = Config_Ptr->compare_value;

			/* Enable Timer0 COMPARE Interrupt */
			TIMSK |= (1<<OCIE2);
		}

		/* adjust clock mode */
		if(Config_Ptr->clock == F_CPU_CLOCK)
		{
			TCCR2 |= (1<<CS20);
		}
		else if(Config_Ptr->clock == F_CPU_8)
		{
			TCCR2 |= (1<<CS21);
		}
		else if(Config_Ptr->clock == F_CPU_32)
		{
			TCCR2 |= (1<<CS20) | (1<<CS21);
		}
		else if(Config_Ptr->clock == F_CPU_64)
		{
			TCCR2 |= (1<<CS22);
		}
		else if(Config_Ptr->clock == F_CPU_256)
		{
			TCCR2 |= (1<<CS21) | (1<<CS22);
		}
		else if(Config_Ptr->clock == F_CPU_1024)
		{
			TCCR2 |= (1<<CS20) | (1<<CS21) | (1<<CS22);
		}

	}


}

/*
 * Description: Function to set the Call Back function address.
 */
void Timer_setCallBack(void(*a_ptr)(void))
{

	/* Save the address of the Call back function in a global variable */
	if(g_timer_number == Timer0)
	{
		g_Timer0callBackPtr = a_ptr;
	}
	else if(g_timer_number == Timer1)
	{
		g_Timer1callBackPtr = a_ptr;
	}
	else if(g_timer_number == Timer2)
	{
		g_Timer2callBackPtr = a_ptr;
	}

}

/*
 * Description: Function to stop the timer.
 */

void Timer_stop(uint8 a_timer_number)
{
	if(a_timer_number == Timer0)
	{
		TCCR0 = 0;
	}
	else if(a_timer_number == Timer1)
	{
		TCCR1B = 0;
	}
	else if(a_timer_number == Timer2)
	{
		TCCR2 = 0;
	}

}

