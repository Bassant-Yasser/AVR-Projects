/*----------------------------------------------------------------------------------------------
 * File Name: HMI.c
 *
 * AUTHOR: Bassnat Yasser
 *
 * Data Created: 28 / 3 / 2021
 *
 * Description: Main file which controls what will be displayed on the screen and manipulates
 * 				data got from the user.
 ------------------------------------------------------------------------------------------------*/


#define F_CPU 8000000UL

#include "lcd.h"
#include "uart.h"
#include "keypad.h"

#define M1_READY 0x20
#define M2_READY 0x10
#define MAX_PASSWORD 5
#define VALIDATE 0x01
#define CHANGE_PASS 0x00
#define OPEN_DOOR 0x10
#define EMERGENCY 0x11

							/***************** FUNCTION PROTYPES *******************/
void get_password(uint8 password[MAX_PASSWORD], char *Str);
void get_2passwords(uint8 password[MAX_PASSWORD], uint8 re_password[MAX_PASSWORD]);
uint8 sequence_1(uint8 action);

							/*****************  GLOBAL VARIABLES *******************/
uint8 g_password[MAX_PASSWORD];
uint8 g_repassword[MAX_PASSWORD];
uint8 g_match = FALSE;
uint8 g_counter = 0;

int main(void)
{
	uint8 key;

	/* Create configuration structure for UART driver */
	UART_ConfigType uart_config_Ptr = {EIGHT_BITS, DISABLED, ONE_STOP_BIT, 9600};

	/* Initialize both the LCD and UART driver */
	UART_init(&uart_config_Ptr);
	LCD_init();

	/* UART sends to microcontroller2 that it is ready */
	UART_sendByte(M1_READY);
	g_match = UART_recieveByte();

	/* Take password from user if it is first time */
	if(g_match == FALSE)
	{
		sequence_1(VALIDATE);
	}


    while(1)
    {
    	/* Display main options */
    	LCD_clearScreen();
    	LCD_displayString("+: Change pass");
    	LCD_goToRowColumn(1, 0);
    	LCD_displayString("-: Open door");

    	/* Wait until user chooses an option */
    	do
    	{
    		key = KeyPad_getPressedKey();
    		_delay_ms(500);
    	}
    	while(key != '+' && key != '-');

    	/* If user chooses to change password then get current password first then get the new password */
    	if(key == '+')
    	{
    		g_counter = 0;
    		g_match = FALSE;

    		/* Give the user 3 chances to enter current password correct */
    		while(g_match != TRUE && g_counter != 3)
    		{
    			g_counter++;

    			/* Get password from user and saves t in in g_password variable */
    	  		get_password(g_password, "Enter old password");

    	  		/* Wait until user press '=' */
				do
				{
					key = KeyPad_getPressedKey();
					_delay_ms(500);
				}
				while(key != '=');

				/* Wait until microcontroller2 is ready and then send to it VALIDATE action */
				while(UART_recieveByte() != M2_READY);
				UART_sendByte(VALIDATE);

				/* UART sends password to microcontroller2 to validate it */
				for (int i = 0; i < MAX_PASSWORD; i++)
				{
					UART_sendByte(g_password[i]);
					_delay_ms(50);
				}

				/* UART sends to microcontroller2 that it is ready to receive result of validation */
				UART_sendByte(M1_READY);

				/* UART receives result of validation */
				g_match = UART_recieveByte();
    		}

    		/* If passwords are matched then get the new password from user */
    		if(g_match == TRUE)
    		{
    			/* Get the 2 passwords from user and return the result of validation */
    			g_match = sequence_1(CHANGE_PASS);

    			/* If 2 passwords are matched display on the screen that password is changed */
    			if(g_match == TRUE)
    			{
    				LCD_clearScreen();
					LCD_displayString("Password is");
					LCD_goToRowColumn(1, 0);
					LCD_displayString("changed");
					_delay_ms(1500);
    			}

    			/* If passwords are not matched then g_match will be false and user will try again until
    			 * counter is 3 */
    		}

    		/* If g_match is still false indicating that the user tries 3 times */
    		if(g_match == FALSE)
    		{
    			LCD_clearScreen();

    			/* Wait until microcontroller2 is ready and then send to it EMERGENCY action */
	    		while(UART_recieveByte() != M2_READY);
	    		UART_sendByte(EMERGENCY);

	    		/* Display on lcd ERROR message */
    			LCD_displayString("ERROR");

    			/* Wait for 1 minute */
    			for(int i = 1; i <= 60; i++)
    			{
    				_delay_ms(1000);
    			}
    		}


    	}
    	/* If user chooses to open door then get current password first */
    	else if (key == '-')
    	{
    		g_match = FALSE;
    		g_counter = 0;

    		/* Give the user 3 chances to enter current password correct */
    		while(g_match != TRUE && g_counter != 3)
    		{
    			g_counter++;

    			/* Get password from user and saves t in in g_password variable */
        		get_password(g_password, "Enter password");

        		/* Wait until user press '=' */
        		do
        		{
        			key = KeyPad_getPressedKey();
        			_delay_ms(300);
        		}
        		while(key != '=');

        		/* Wait until microcontroller2 is ready and then send to it OPEN_DOOR action */
        		while(UART_recieveByte() != M2_READY);
        		UART_sendByte(OPEN_DOOR);

        		/* UART sends password to microcontroller2 to validate it */
        		for (int i = 0; i < MAX_PASSWORD; i++)
        		{
        			UART_sendByte(g_password[i]);
        			_delay_ms(50);
        		}

        		/* UART sends to microcontroller2 that it is ready to receive result of validation */
        		UART_sendByte(M1_READY);

        		/* UART receives result of validation */
        		g_match = UART_recieveByte();

        		/* If passwords are matched then display on lcd door is unlocking */
        		if(g_match == TRUE)
        		{
        			LCD_clearScreen();

        			/* Displays on lcd that door is unlocking */
        			LCD_displayString("Door is");
        			LCD_goToRowColumn(1, 0);
        			LCD_displayString("unlocking");

        			/* Wait until microcontroller2 sends byte indicating that the door will start closing */
        			while(UART_recieveByte() != M2_READY);

					LCD_clearScreen();

					/* Displays on lcd that door is locking */
					LCD_displayString("Door is locking");

					/* Wait until microcontroller2 sends byte indicating that the door is closed */
					while(UART_recieveByte() != M2_READY);
        		}

        		/* Give the user 3 tries to enter the current password correctly */
        		else if(g_match == FALSE && g_counter != 3)
        		{
        			LCD_clearScreen();
        			LCD_displayString("Not matched");
        			LCD_goToRowColumn(1, 0);
        			LCD_displayString("Try again");
        			_delay_ms(1500);
        		}
    		}
    		/* If g_match is still false indicating that the user tries 3 times */
    		if(g_counter == 3 && g_match == FALSE)
    		{
    			LCD_clearScreen();

    			/* Wait until microcontroller2 is ready and then send to it EMERGENCY action */
        		while(UART_recieveByte() != M2_READY);
        		UART_sendByte(EMERGENCY);

        		/* Display on lcd ERROR message */

        		LCD_displayString("ERROR");

        		/* Wait for 1 minute */
				for(int i = 1; i <= 60; i++)
				{
					_delay_ms(1000);
				}
    		}

    	}

    }
}

/*
 * Function Name: sequence_1
 * Description:  Function takes 2 passwords and sends them to microcontroller2 and gets
 * 				 the result of validation
 * [in]: action : indicate VALIDATE or CHANGE_PASS
 * [out]: The result of comparison
 */

uint8 sequence_1(uint8 a_action)
{
	g_counter = 0;
	g_match = FALSE;

	/* Give the user 3 chances to enter the 2 passwords matched */
	while(g_match != TRUE && g_counter != 3)
	{
		g_counter++;

		/* CALL get_2passwords function */
		get_2passwords(g_password, g_repassword);

		/* Wait until microcontroller2 is ready and then send to it  a_action */
		while(UART_recieveByte() != M2_READY){}
		UART_sendByte(a_action);

		/* UART sends password to microcontroller2 */
		for (int i = 0; i < MAX_PASSWORD; i++)
		{
			UART_sendByte(g_password[i]);
			_delay_ms(50);
		}

		/* UART sends repeated password to microcontroller2 */
		for (int i = 0; i < MAX_PASSWORD; i++)
		{
			UART_sendByte(g_repassword[i]);
			_delay_ms(50);
		}

		/* UART sends to microcontroller2 that it is ready to receive result of validation */
		UART_sendByte(M1_READY);

		/* UART receives result of validation */
		g_match = UART_recieveByte();

	}
	return g_match;
}

/*
 * Function Name: get_2passwords
 * Description:  Function takes 2 passwords from the user
 * [in]: 2 arrays to saves passwords in them
 */

void get_2passwords(uint8 a_password[MAX_PASSWORD], uint8 a_repassword[MAX_PASSWORD])
{
	/* Get password from the user */
	get_password(a_password, "Enter new pass");

	/* Wait until user press '=' */
	while(KeyPad_getPressedKey() != '=');

	/* Get repeated password from the user */
	get_password(a_repassword, "Re_enter pass");

	/* Wait until user press '=' */
	while(KeyPad_getPressedKey() != '=');

	LCD_clearScreen();
	LCD_displayString("WAIT...");

}

/*
 * Function Name: get_password
 * Description:  Function takes password from the user
 * [in]: one array to saves password in it
 */

void get_password(uint8 a_password[MAX_PASSWORD], char *Str)
{
	LCD_clearScreen();
	LCD_goToRowColumn(0, 0);
	LCD_displayString(Str);
	LCD_goToRowColumn(1, 0);
	LCD_displayString("*****");
	LCD_goToRowColumn(1, 0);
	/* Get password from user */
	for(int i = 0; i < MAX_PASSWORD; i++)
	{
		a_password[i] = KeyPad_getPressedKey();

		/* Saves what user pressed on it if it is a number */
		if(a_password[i] >= 0 && a_password[i] <= 9)
		{
			LCD_intgerToString(a_password[i]);
		}
		else
		{
			i--;
		}
		_delay_ms(500);

	}
}

