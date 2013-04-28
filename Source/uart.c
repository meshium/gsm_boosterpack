/*
 * uart.c
 *
 *  Created on: 10.03.2013
 *      Author: al
 */

#include "msp430g2553.h"
#include "interrupts.h"
#include "uart.h"
#include "buffer.h"
#include "string.h"

cBuffer txBuf;
cBuffer rxBuf;

char rxData[ RX_BUF_MAX_SIZE ];
char txData[ TX_BUF_MAX_SIZE ];

volatile int uartRxOverflow;

void initUart( void )
{

	initUsci2Uart();

	bufferInit( &rxBuf, rxData, RX_BUF_MAX_SIZE );	// Initialize receiving buffer
	bufferInit( &txBuf, txData, TX_BUF_MAX_SIZE );	// Initialize transmitting buffer

	attachUsciRx( uartBufferedRx );
	attachUsciTx( uartBufferedTx );

	uartRxOverflow = 0;
}

void uartBufferedRx( char data )
{
	if ( !bufferWrite( &rxBuf, data ) )
		uartRxOverflow++;
}

int uartBufferedTx( char *data )
{
	if ( bufferRead( &txBuf, data ) )
		return TRUE;

	return FALSE;
}

// Return reference to UART receive buffer
cBuffer* uartGetRxBuffer( void )
{
	return &rxBuf;
}

// Return reference to UART transmit buffer
cBuffer* uartGetTxBuffer( void )
{
	return &txBuf;
}


// Put string to UART transmit buffer
int writeTxBuffer( const char *str )
{

	unsigned int i, length;

	length = strlen( str );

	for( i = 0; i < length; i++)
		bufferWrite( &txBuf, str[i] );
}

// Put buffered data to str and return length
int readRxBuffer( char *str )
{
	unsigned int i;

	i = 0;

	while ( rxBuf.data_len )
	{
		if ( !bufferRead( &rxBuf, &str[ i++ ] ) )
		{
//			 panic( RUNTIME_ERROR );
		}
	}

	str[ i ] = '\0';

	return i;
}

// Clear UART transmit buffer
void clearTxBuffer( void )
{
	bufferFlush( &txBuf );
}

// Clear UART receive buffer
void clearRxBuffer( void )
{
	bufferFlush( &rxBuf );
	uartRxOverflow = 0;
}

// Initialize transmitting data from TX buffer
void flushTxBuffer( void )
{

	if ( !bufferEmpty( &txBuf ) )
		STI_USCITX;
}

int txBufferEmpty( void )
{
	return bufferEmpty( &txBuf );
}

int rxBufferEmpty( void )
{
	return bufferEmpty( &rxBuf );
}

int rxBufferOverflow( void )
{
	return uartRxOverflow;
}
