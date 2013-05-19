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

#define RX_BUF_MAX_SIZE 80
#define TX_BUF_MAX_SIZE 80

void initUart( void );
int uartBufferedTx( char *data );
void uartBufferedRx( char data );
int writeTxBuffer( const char *str );
int readRxBuffer( char *str );
int sendData( const char *str );
void clearTxBuffer( void );
void clearRxBuffer( void );
void flushTxBuffer( void );
int txBufferEmpty( void );
int rxBufferEmpty( void );
int rxBufferOverflow( void );

cBuffer* uartGetRxBuffer( void );
cBuffer* uartGetTxBuffer( void );

#endif /* UART_H_ */
