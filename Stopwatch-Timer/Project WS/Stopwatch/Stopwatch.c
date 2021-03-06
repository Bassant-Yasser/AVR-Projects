

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void Timer1_Init();
void Int0_Init(void);
void Int1_Init(void);
void Int2_Init(void);
void display(unsigned char);


unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;
unsigned char Timer1_CompA_Flag = 0;	/* Flag to indicate that timer1 counts 1 second */


int main()
{
	Int0_Init();		/* Enable external INT0 */
	Int1_Init();		/* Enable external INT1 */
	Int2_Init();		/* Enable external INT2 */

	DDRC |= 0x0f;		/* configure first 4 pins in PORTC as output */
	PORTC &= 0xf0;		/* initialize the 7-segment with value 0 by clear the first four bits in PORTC */

	DDRA |= 0x3f;		/* configure first 6 pins in PORTA as output */
	PORTA &= 0xC0;		/* Disable all the 7-segments */
	PORTA |= 0x01;		/* Enable first 7-segment */


	Timer1_Init();		/* Enable timer1 compare mode */


	int i = 0;

	while(1)
	{
		if(Timer1_CompA_Flag)
		{
			Timer1_CompA_Flag = 0;
			seconds++;
			if(seconds == 60)
			{
				seconds = 0;
				minutes++;
				if(minutes == 60)
				{
					minutes = 0;
					hours++;
					if(hours == 24)
					{
						seconds = 0;
						minutes = 0;
						hours = 0;
					}
				}
			}
		}

		for(i = 1; i <= 6; i++)
		{
			display(i);
			_delay_ms(2);

		}
	}
}


void display(unsigned char number_of_7segment)
{
	unsigned char temp = 0;
	unsigned enable = 1;	/* Indicate which 7-segment to enable */
	enable = enable << (number_of_7segment - 1);

	PORTA = (PORTA & 0xC0) | (enable & 0x3f);

	if(number_of_7segment == 1)
	{
		temp = seconds % 10;
		PORTC = (PORTC & 0xf0) | (temp & 0x0f);
	}
	else if(number_of_7segment == 2)
	{
		temp = seconds / 10;
		PORTC = (PORTC & 0xf0) | (temp & 0x0f);
	}
	else if(number_of_7segment == 3)
	{
		temp = minutes % 10;
		PORTC = (PORTC & 0xf0) | (temp & 0x0f);
	}

	else if(number_of_7segment == 4)
	{
		temp = minutes / 10;
		PORTC = (PORTC & 0xf0) | (temp & 0x0f);
	}

	else if(number_of_7segment == 5)
	{
		temp = hours % 10;
		PORTC = (PORTC & 0xf0) | (temp & 0x0f);
	}

	else if(number_of_7segment == 6)
	{
		temp = hours / 10;
		PORTC = (PORTC & 0xf0) | (temp & 0x0f);
	}
}

void Timer1_Init()
{
	TCCR1A |= 0;

	/* Configure timer control register TCCR1B
	 * 1. CTC Mode WGM12=1 WGM13=0 (Mode Number 4)
	 * 2. Prescaler = F_CPU/1024 CS10=1 CS11=0 CS12=1
	 */
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);

	TIMSK = (1<<OCIE1A);	/* Enable Timer1 Compare A Interrupt */
	SREG |= (1<<7);			/* Enable interrupts by setting I-bit */

	TCNT1 = 0;				/* Set timer1 initial count to zero */
	OCR1A = 975;			/* Set the Compare value to 975 */

}

void Int0_Init(void)
{
	DDRD &= ~(1<<2);		/* Configure pin 2 in PORTD as input pin */
	PORTD |= (1<<2);		/* Activate the internal pull up resistor at PD2 */

	GICR |= (1<<INT0);		/* Enable external interrupt pin INT0 */
	MCUCR |= (1<<ISC01);	/* Trigger INT0 with the falling edge */
	SREG |= (1<<7);			/* Enable interrupts by setting I-bit */
}

void Int1_Init(void)
{
	DDRD &= ~(1<<3);					/* Configure pin 3 in PORTD as input pin */

	GICR |= (1<<INT1);					/* Enable external interrupt pin INT1 */
	MCUCR |= (1<<ISC11) | (1<<ISC10);	/* Trigger INT1 with the raising edge */
	SREG |= (1<<7);						/* Enable interrupts by setting I-bit */
}

void Int2_Init(void)
{
	DDRB &= ~(1<<2);		/* Configure pin 2 in PORTB as input pin */
	PORTB |= (1<<2);		/* Activate the internal pull up resistor at PB2 */

	GICR |= (1<<INT2);		/* Enable external interrupt pin INT2 */
	MCUCSR &= ~(1<<ISC2);	/* Trigger INT2 with the falling edge */
	SREG |= (1<<7);			/* Enable interrupts by setting I-bit */
}



ISR (TIMER1_COMPA_vect)
{
	Timer1_CompA_Flag = 1;

}

ISR (INT0_vect)
{
	TCCR1B |= (1<<CS10) | (1<<CS12);	/* Enable Timer clock by  prescaler 1024 */
	TCNT1 = 0;							/* Set timer1 count to zero */
	Timer1_CompA_Flag = 0;				/* Set timer compare flag to zero */
	seconds = 0;
	minutes = 0;
	hours = 0;
}

ISR (INT1_vect)
{
	TCCR1B &= 0xf8;		/* Disable Timer clock by set 3 LSBs to zero */
	TCNT1 = 0;				/* Set timer1 count to zero */
}


ISR (INT2_vect)
{
	TCCR1B |= (1<<CS10) | (1<<CS12);	/* Enable Timer clock by  prescaler 1024 */
}


