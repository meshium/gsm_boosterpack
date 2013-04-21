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

void initUart(void)
{
	P1SEL |= RX + TX;
	P1SEL2 |= RX + TX;

	UCA0CTL1 |= UCSSEL_2;
	UCA0BR0 = 104;
	UCA0BR1 = 0;
	UCA0MCTL = UCBRF_3 + UCOS16;
	UCA0CTL1 &= ~UCSWRST;

	bufferInit( &rxBuf, rxData, RX_BUF_MAX_SIZE );
	bufferInit( &txBuf, txData, TX_BUF_MAX_SIZE );

	UartRxFunc = 0;

	IE2 |= UCA0RXIE;
}

cBuffer* uartGetRxBuffer()
{
	return &rxBuf;
}

cBuffer* uartGetTxBuffer()
{
	return &txBuf;
}

void uartSetRxFunc( void( *rx_func )( char c ) )
{
	UartRxFunc = rx_func;
}

int writeTxBuffer( const char *str )
{

	unsigned int i, length;

	length = strlen( str );

	for( i = 0; i < length; i++)
		bufferWrite( &txBuf, str[i] );

}

int readRxBuffer( char *str, unsigned int length )
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
	uartGotLine = FALSE;

	return i;

}

void clearTxBuffer()
{
	bufferFlush( &txBuf );
}

void clearRxBuffer()
{
	bufferFlush( &rxBuf );
}

void flushTxBuffer()
{

	if ( !bufferEmpty( &txBuf ) )
		STI_UARTTX;

}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{

	char data;

	if ( bufferRead( &txBuf, &data ) )
		UCA0TXBUF = data;
	else
		CLI_UARTTX;

}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{

	char data;

	data = UCA0RXBUF;

	if ( !bufferWrite( &rxBuf, data ) )
		uartRxOverflow++;

	if ( UartRxFunc )
		UartRxFunc( data );

}

