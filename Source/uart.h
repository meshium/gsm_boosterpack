/*
 * uart.h
 *
 *  Created on: 10.03.2013
 *      Author: al
 */

#include "global.h"
#include "buffer.h"

#ifndef UART_H_
#define UART_H_

#define RX BIT1
#define TX BIT2

#define CLI_UARTTX IE2 &= ~UCA0TXIE;
#define STI_UARTTX IE2 |= UCA0TXIE;

#define RX_BUF_MAX_SIZE 100
#define TX_BUF_MAX_SIZE 80

void initUart();
void uartSetRxFunc( void( *rx_func )( char c ) );
int writeTxBuffer( const char *str );
int readRxBuffer( char *str );
void clearTxBuffer( void );
void clearRxBuffer( void );
void flushTxBuffer( void );

cBuffer* uartGetRxBuffer( void );
cBuffer* uartGetTxBuffer( void );

#endif /* UART_H_ */
