/*----------------------------------------------------------------------------------------------
 * File Name: Control_M2.c
 *
 * AUTHOR: Bassnat Yasser
 *
 * Data Created: 28 / 3 / 2021
 *
 * Description: Main file which validates password and saves it and controls motor.
 ------------------------------------------------------------------------------------------------*/

#define F_CPU 8000000UL

#include "uart.h"
#include "lcd.h"
#include "external_eeprom.h"
#include "timer.h"

#define M1_READY 0x20
#define M2_READY 0x10
#define MAX_PASSWORD 5
#define START_ADDRESS 0x0311
#define FIRST_OPEN_ADDRESS 0x302
#define VALIDATE 0x01
#define CHANGE_PASS 0x00
#define OPEN_DOOR 0x10
#define EMERGENCY 0x11
#define NUMBER_OF_OVERFLOWS_PER_SECOND_Timer0_2 32
#define NUMBER_OF_OVERFLOWS_PER_SECOND_Timer1 15

/* global variable contain the ticks count of the timer */
uint16 g_tick = 0;

/* global variable contain the state of door */
uint8 g_state = 0;

/* Create configuration structure for Timer driver */
Timer_ConfigType Config_Ptr;

void door();

uint8 validate_password(const uint8 * password, const uint8 * re_password);

int main(void)
{
	uint8 password[MAX_PASSWORD];
	uint8 re_password[MAX_PASSWORD];
	uint8 old_password[MAX_PASSWORD];
	uint8 match = FALSE;
	uint8 action;

	/* Enable Global Interrupt I-Bit */
	SREG |= (1<<7);

	/* Create configuration structure for UART driver */
	UART_ConfigType uart_config_ptr = {EIGHT_BITS, DISABLED, ONE_STOP_BIT, 9600};

	/* Initialize both the UART and EEPROM driver */
	UART_init(&uart_config_ptr);
	LCD_init();
	EEPROM_init();

	/* configure pin PC6 and PC7 (motors pins) as output pins */
	SET_BIT(DDRB, PB6);
	SET_BIT(DDRB, PB7);

	/* Motor is stop at the beginning */
	CLEAR_BIT(PORTB, PB6);
	CLEAR_BIT(PORTC, PB7);

	/* configure pin PD3 (led pin) as output pin */
	SET_BIT(DDRD, PD3);

	/* led is off at the beginning (positive logic) */
	CLEAR_BIT(PORTD, PD3);

	EEPROM_readByte(FIRST_OPEN_ADDRESS, &match);
	if(match == 0x0A)
	{
		/* Wait until microcontroller1 is ready */
		while(UART_recieveByte() != M1_READY);
		UART_sendByte(TRUE);
	}
	else if(match != 0x0A)
	{
		/* Wait until microcontroller1 is ready and then send to it the result of validation */
		while(UART_recieveByte() != M1_READY);
		UART_sendByte(FALSE);

		match = FALSE;
		while(match != TRUE)
		{
			/* UART sends to microcontroller1 that it is ready to receive data from it */
			UART_sendByte(M2_READY);

			/* UART receives action (VALIDATE) to validate the 2 passwords */
			action = UART_recieveByte();

			/* UART receives password and saves it */
			for (int i = 0; i < MAX_PASSWORD; i++)
			{
				password [i] = UART_recieveByte();
			}

			/* UART receives repeated password and saves it */
			for (int i = 0; i < MAX_PASSWORD; i++)
			{
				re_password [i] = UART_recieveByte();
			}

			/* Checks if 2 passwords are matched and return the result if matched or not */
			match = validate_password(password, re_password);

			/* If passwords are matched then will saves password in EE2PROM */
			if(match == TRUE)
			{
				for(int i = 0; i < MAX_PASSWORD; i++)
				{
					EEPROM_writeByte(START_ADDRESS + i, password[i]);
					_delay_ms(10);
				}
			}

			/* Wait until microcontroller1 is ready and then send to it the result of validation */
			while(UART_recieveByte() != M1_READY);
			UART_sendByte(match);
		}

		EEPROM_writeByte(FIRST_OPEN_ADDRESS,0x0A);
	}


    while(1)
    {
    	/* UART sends to microcontroller1 that it is ready to receive data from it */
    	UART_sendByte(M2_READY);

    	/* UART receives action*/
    	action = UART_recieveByte();

    	/* If action is to validate then will receive password and compare t with saved password in EE2PROM */
    	if(action == VALIDATE)
    	{
    		/* UART receives the password from mictocontroller1 to check it */
    		for (int i = 0; i < MAX_PASSWORD; i++)
    		{
    			password [i] = UART_recieveByte();
    		}

    		/* Read the saved password from EE2PROM to compare it with received password */
    		for(int i = 0; i < MAX_PASSWORD; i++)
    		{
    			EEPROM_readByte(START_ADDRESS + i, &old_password[i]);
    			_delay_ms(10);
    		}

    		/* Checks if 2 passwords are matched and return the result if matched or not */
    		match = validate_password(password, old_password);

    		/* Wait until microcontroller1 is ready and then send to it the result of validation */
    		while(UART_recieveByte() != M1_READY);
    		UART_sendByte(match);
    	}
    	/* If the action is to change password then it will receives 2 passwords and validates them if they
    	 * are matched then will save the new password in EE2PROM */
    	else if(action == CHANGE_PASS)  // change password
		{
    		/* UART receives password and saves it */
    		for (int i = 0; i < MAX_PASSWORD; i++)
			{
				password [i] = UART_recieveByte();
			}

    		/* UART receive repeated password and saves it */
    		for (int i = 0; i < MAX_PASSWORD; i++)
			{
				re_password [i] = UART_recieveByte();
			}

    		/* Checks if 2 passwords are matched and return the result if matched or not */
    		match = validate_password(password, re_password);

    		/* If passwords are matched then will saves password in EE2PROM */
    		if(match == TRUE)
    		{
    			for(int i = 0; i < MAX_PASSWORD; i++)
    			{
    				EEPROM_writeByte(START_ADDRESS + i, password[i]);
    				_delay_ms(10);
    			}
    		}

    		/* Wait until microcontroller1 is ready and then send to it the result of validation */
    		while(UART_recieveByte() != M1_READY);
    		UART_sendByte(match);
		}
    	/* If action is to open door then microcontroller2 will receives the current password from
    	 * microcontroller1 and compare it with saved password n EE2PROM if matched will open door */
    	else if(action == OPEN_DOOR)
    	{
    		/* UART receives the password from mictocontroller1 to check it */
    		for (int i = 0; i < MAX_PASSWORD; i++)
			{
				password [i] = UART_recieveByte();
			}

    		/* Read the saved password from EE2PROM to compare it with received password */
    		for(int i = 0; i < MAX_PASSWORD; i++)
    		{
    			EEPROM_readByte(START_ADDRESS + i, &old_password[i]);
    			_delay_ms(10);
    		}

    		/* Checks if 2 passwords are matched and return the result if matched or not */
    		match = validate_password(password, old_password);

    		/* If 2 passwords are matched, start the timer to count 15 seconds and rotate motor clockwise */
    		if(match == TRUE)
    		{
    			/* Adjust configuration of Timer */
    			Config_Ptr.mode = NORMAL;
    			Config_Ptr.initial_value = 0;
    			Config_Ptr.compare_value = 0;

    			/* Use these to adjust Timer0 or Timer 2 to start counting */
    			Config_Ptr.timer_number = Timer2;
    			Config_Ptr.clock = F_CPU_1024;

    			/* Use these to adjust  Timer1 to start counting */

    			/*
    			 *  Config_Ptr.timer_number = Timer1;
    			 *  Config_Ptr.clock = F_CPU_8;
    			 */

    			/* Initialize timer */
    			Timer_init(&Config_Ptr);

    			/* Rotate motor clock wise */
    			CLEAR_BIT(PORTB, PB6);
    			SET_BIT(PORTB, PB7);

    			/* Set the Call back function pointer in the Timer driver */
    			Timer_setCallBack(door);

    			/* Wait until microcontroller1 is ready and then send to it the result of validation */
        		while(UART_recieveByte() != M1_READY);
        		UART_sendByte(TRUE);

        		/* Wait until door is open and then closed */
        		while(g_state != 3);

        		/* Return the state of door to ideal to be able to start this operation again */
        		g_state = 0;
    		}
    		/* If passwords are not matched then return to microcontroller1 the result of match */
    		else if(match == FALSE)
    		{
    			/* Wait until microcontroller1 is ready and then send to it the result of validation */
        		while(UART_recieveByte() != M1_READY);
        		UART_sendByte(FALSE);
    		}
    	}
    	/* If action is emergency, led will be open for 1 minute */
    	else if(action == EMERGENCY)
    	{
    		/* Led is on */
    		SET_BIT(PORTD, PD3);

    		 /* Wait for 1 minute */
    		for(int i = 1; i <= 60; i++)
			{
				_delay_ms(1000);
			}

    		/* Led is off */
    		CLEAR_BIT(PORTD, PD3);
    	}

    }
}

/*
 * Function Name: validate_password
 * Description:  Function takes 2 passwords and compares the if they are matched will return TRUE
 * 				 and if they are not matched will return FALSE
 * [in]: address of array password1, and address of array password2
 * [out]: The result of comparison
 */
uint8 validate_password(const uint8 * password, const uint8 * re_password)
{
	for (int i = 0; i < MAX_PASSWORD; i++)
	{
		if(password[i] != re_password[i])
		{
			return ERROR;
		}
	}
	return SUCCESS;
}


void door()
{
	/* Increment g_tick every time this function will be called */
	g_tick++;

	/* If timer counts 15 seconds stop the motor */
	if(g_tick == (NUMBER_OF_OVERFLOWS_PER_SECOND_Timer0_2 * 15) && g_state == 0)
	{
		/* Stop motor */
		CLEAR_BIT(PORTB, PC6);
		CLEAR_BIT(PORTB, PC7);

		/* Return g_tick to zero to start counting 3 seconds and go to g_state = 1 */
		g_tick = 0;
		g_state = 1;

	}
	/* If timer counts 2 seconds , rotate motor anti-clock wise */
	else if(g_tick == (NUMBER_OF_OVERFLOWS_PER_SECOND_Timer0_2 * 3) && g_state == 1)
	{
		/* UART sends byte to microcontroller1 to indicate that the door will start closing after 3 seconds */
		UART_sendByte(M2_READY);

		/* Rotate motor anti-clock wise */
		SET_BIT(PORTB, PB6);
		CLEAR_BIT(PORTB, PB7);

		/* Return g_tick to zero to start counting 3 seconds and go to g_state = 2 */
		g_tick = 0;
		g_state = 2;
	}
	/* If timer counts 15 seconds , stop motor */
	else if(g_tick == (NUMBER_OF_OVERFLOWS_PER_SECOND_Timer0_2 * 15) && g_state == 2)
	{
		/* UART sends byte to microcontroller1 to indicate that the door is closed */
		UART_sendByte(M2_READY);

		/* Stop motor */
		CLEAR_BIT(PORTB, PB6);
		CLEAR_BIT(PORTB, PB7);

		/* Return g_tick to zero and go to g_state = 3 */
		g_tick = 0;
		g_state = 3;

		/* Stop the timer */
		Timer_stop(Timer2);
	}

}
