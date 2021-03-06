/*
 * Mini_Project2.c
 *
 *  Created on: Feb 19, 2021
 *      Author: besos
 */

#include "lcd.h"
#include "adc.h"

void PWM_Timer0_Init(uint8 set_duty_cycle);
void INT1_init();

int main(void)
{
	uint16 data;

	INT1_init();	/* Enable external INT0 */

	ADC_ConfigType ADC_Config = {AREF, Division_Factor_8};
	ADC_init(&ADC_Config);	/* initialize ADC driver */

	LCD_init();	/* initialize LCD driver */
	LCD_clearScreen(); /* clear LCD at the beginning */
	/* display this string "ADC Value = " only once at LCD */
	LCD_displayString("ADC Value = ");

	/* configure pin PB0 and PB1 as output pins */
	DDRB |= 0x03;
	/* Rotate the motor --> clock wise */
	CLEAR_BIT(PORTB, PB0);
	SET_BIT(PORTB, PB1);

	while(1)
	{
		data = ADC_readChannel(PA0);	/* read channel zero where the potentiometer is connect  */
		LCD_goToRowColumn(0, 12);	/* display the number every time at this position  */
		LCD_intgerToString(data);	/* display the ADC value on LCD screen  */
		data = (data * 256UL) / 1024UL;	/* calculate the duty cycle from the ADC value */
		PWM_Timer0_Init(data);	/* generate duty cycle equivalent to value read from potentiometer*/
	}
}


void PWM_Timer0_Init(uint8 set_duty_cycle)
{
	TCNT0 = 0;	/* Set Timer Initial value */

	OCR0 = set_duty_cycle;	/* Set Compare Value */

	SET_BIT(DDRB, PB3);	/* set PB3/OC0 as output pin --> pin where the PWM signal is generated from MC. */

	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);


}

void INT1_init()
{
	CLEAR_BIT(DDRD, PD3);	/* Configure INT0/PD3 as input pin */
	MCUCR = (1<<ISC10) | (1<<ISC11);	/* Trigger INT1 with the raising edge */
	SET_BIT(GICR, INT1);	/* Enable external interrupt pin INT1 */
	SET_BIT(SREG, 7);	/* Enable interrupts by setting I-bit */

}

ISR(INT1_vect)
{
	/* Rotate the motor --> opposite to its direction */
	TOGGLE_BIT(PORTB, PB0);
	TOGGLE_BIT(PORTB, PB1);
}
