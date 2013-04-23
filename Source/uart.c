/*
 * uart.c
 *
 *  Created on: 10.03.2013
 *      Author: al
 */

#include "msp430g2553.h"
#include "uart.h"
#include "buffer.h"
#include "string.h"

typedef void( *voidFuncPtr )( char );
static volatile voidFuncPtr UartRxFunc;

cBuffer txBuf;
cBuffer rxBuf;

char rxData[ RX_BUF_MAX_SIZE ];
char txData[ TX_BUF_MAX_SIZE ];

volatile int uartRxOverflow;

void initUart( void )
{
	P1SEL |= RX + TX;				// USCIA0
	P1SEL2 |= RX + TX;				// P1.1 - Rxd, P1.2 - Txd

	UCA0CTL1 |= UCSSEL_2;			// SMCLK
	UCA0BR0 = 104;					// 16MHz 9600
	UCA0BR1 = 0;					//
	UCA0MCTL = UCBRF_3 + UCOS16;	//
	UCA0CTL1 &= ~UCSWRST;			// Initialize USCI state machine

	bufferInit( &rxBuf, rxData, RX_BUF_MAX_SIZE );	// Initialize receiving buffer
	bufferInit( &txBuf, txData, TX_BUF_MAX_SIZE );	// Initialize transmitting buffer

	UartRxFunc = 0;					// No user defined received data processing

	IE2 |= UCA0RXIE;				// Enable USCIA0 rx interrupt
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

// Attach user defined received data postprocessing function
void uartSetRxFunc( void( *rx_func )( char c ) )
{
	UartRxFunc = rx_func;
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
void clearTxBuffer()
{
	bufferFlush( &txBuf );
}

// Clear UART receive buffer
void clearRxBuffer()
{
	bufferFlush( &rxBuf );
}

// Initialize transmitting data from TX buffer
void flushTxBuffer()
{

	if ( !bufferEmpty( &txBuf ) )
		STI_UARTTX;

}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR( void )
{

	char data;

	if ( bufferRead( &txBuf, &data ) )
		UCA0TXBUF = data;
	else
		CLI_UARTTX;

}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR( void )
{

	char data;

	data = UCA0RXBUF;

	if ( !bufferWrite( &rxBuf, data ) )
		uartRxOverflow++;

	if ( UartRxFunc )
		UartRxFunc( data );

}

