 /******************************************************************************
 *
 * Module: ADC
 *
 * File Name: adc.c
 *
 * Description: Source file for the ATmega16 ADC driver
 *
 * Author: Bassant Yasser
 *
 *******************************************************************************/


#include "ADC.h"

volatile uint16 data;

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void ADC_init(const ADC_ConfigType * Config_Ptr)
{
	ADMUX = (ADMUX & 0x00) | (Config_Ptr->voltage << 6);
	ADCSRA = (1<<ADEN);
	ADCSRA = (ADCSRA & 0xF8) | (Config_Ptr->clock);

#ifdef INTERRUPT_ENABLE
	ADCSRA |=  (1<<ADIE);
#endif

}

#ifdef INTERRUPT_ENABLE
void ADC_readChannel(uint8 channel_num)
#else
uint16 ADC_readChannel(uint8 channel_num)
#endif
{
	ADMUX = (ADMUX & 0xE0) | (channel_num & 0x07);
	SET_BIT(ADCSRA, ADSC);

#ifndef INTERRUPT_ENABLE
	while(BIT_IS_CLEAR(ADCSRA, ADIF)){}
	SET_BIT(ADCSRA, ADIF);
	return ADC;
#endif
}

#ifdef INTERRUPT_ENABLE
ISR(ADC_vect)
{
	data = ADC;
}
#endif
