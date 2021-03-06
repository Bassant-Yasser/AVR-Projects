/*----------------------------------------------------------------------------------------------
 * Module: UART
 *
 * File Name: uart.h
 *
 * AUTHOR: Bassnat Yasser
 *
 * Data Created: 28 / 3 / 2021
 *
 * Description: Header file for the UART AVR driver
 ------------------------------------------------------------------------------------------------*/

#ifndef UART_H_
#define UART_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"

typedef enum
{
	FIVE_BITS,SIXTH_BITS, SEVEN_BITS, EIGHT_BITS
}UART_BIT_DATA;

typedef enum
{
	DISABLED, EVEN_PARITY = 2, ODD_PARITY
}UART_PARITY_MODE;

typedef enum
{
	ONE_STOP_BIT, TWO_STOP_BIT
}UART_STOP_BIT;

typedef struct
{
	UART_BIT_DATA bit_data;
	UART_PARITY_MODE parity;
	UART_STOP_BIT stop;
	uint32 baudrate
}UART_ConfigType;

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

///* UART Driver Baud Rate */
//#define USART_BAUDRATE 9600

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void UART_init(const UART_ConfigType * Config_Ptr);

void UART_sendByte(const uint8 data);

uint8 UART_recieveByte(void);

void UART_sendString(const uint8 *Str);

void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
