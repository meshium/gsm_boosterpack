/*
 * ws6318.c
 *
 *  Created on: 10.03.2013
 *      Author: al
 */

#include "msp430g2553.h"
#include "global.h"
#include "string.h"
#include "ws6318.h"
#include "delay.h"
#include "buffer.h"
#include "uart.h"

volatile status moduleStatus;

volatile unsigned int okStrIdx;
volatile unsigned int errorStrIdx;
volatile unsigned int ringStrIdx;

volatile int gotRing;
volatile int moduleAnswer;

char ok[] = { "OK\r\n" };
char simOK[] = { "\r\n+CPIN: READY\r\n\r\nOK\r\n" };
char networkOK[] = { "\r\n+CREG: 0,1\r\n\r\nOK\r\n" };
char error[] = { "ERROR\r\n" };
char ring[] = { "RING\r\n" };

int moduleInit( void )
{
	int i;

	okStrIdx = 0;
	errorStrIdx = 0;
	ringStrIdx = 0;

	gotRing = FALSE;

	moduleAnswer = NULL;

	uartSetRxFunc( moduleUartActivity );

	for ( i=0; i < INIT_TRIES; i++ )
	{

		if ( ~READY_PORT & READY_PIN )
			moduleReset();
		else
			moduleOn();

		setTimeout( 5000 );
		moduleStatus = OFF;

		while ( !timedOut() )
		{
			if ( ~READY_PORT & READY_PIN )
			{
				moduleStatus = ON;
				break;
			}
		}

		if ( moduleStatus == ON )
		{
			if ( moduleExecuteCommand( "E0", ok ) )
			{
				moduleStatus = COMMUNICATE;

				if ( moduleExecuteCommand( "+CPIN?", simOK ) )
				{
					moduleStatus = SIM_OK;

					return TRUE;

				}
			}
		}
	}

	return FALSE;

}

int moduleConfigureGPIO( unsigned char io, unsigned char dir, unsigned char pull )
{

	char cmd[] = { "+WMGPIOCFG=00,0,0" };

	if ( io >= 1 && io <= 12 && dir < 2 && pull < 2 )
	{
		cmd[11] = io / 10 + 0x30;
		cmd[12] = io % 10 + 0x30;
		cmd[14] = dir + 0x30;
		cmd[16] = pull + 0x30;

		if ( moduleExecuteCommand( cmd, ok ) )
			return TRUE;
	}

	return FALSE;
}

int moduleSetGPIO( unsigned char io, unsigned char val )
{
	char cmd[] = { "+WMGPIO=00,0" };

	if ( io >= 1 && io <= 12 && val < 2 )
	{
		cmd[8] = io / 10 + 0x30;
		cmd[9] = io % 10 + 0x30;
		cmd[11] = val + 0x30;

		if ( moduleExecuteCommand( cmd, ok ) )
			return TRUE;
	}

	return FALSE;
}

void moduleUartActivity( char data )
{

	if ( ringStrIdx | okStrIdx | errorStrIdx )
	{
		if ( ringStrIdx )
		{
			if ( data == ring[ ringStrIdx ] )
			{
				if ( ++ringStrIdx >= strlen( ring ) )
				{
					gotRing = TRUE;
					ringStrIdx = 0;
				}
			}
			else
				ringStrIdx = 0;
		}
		else if ( okStrIdx )
		{
			if ( data == ok[ okStrIdx ] )
			{
				okStrIdx++;

				if ( okStrIdx >= strlen(ok) )
				{
					okStrIdx = 0;
					moduleAnswer = OK;
				}
			}
			else
				okStrIdx = 0;
		}
		else if ( errorStrIdx )
		{
			if ( data == error[ errorStrIdx ] )
			{
				if ( ++errorStrIdx >= strlen( error ) )
				{
					errorStrIdx = 0;
					moduleAnswer = ERROR;
				}
			}
			else
				errorStrIdx = 0;
		}
	}
	else if ( data == error[ 0 ] )
		errorStrIdx++;
	else if ( data == ok[ 0 ] )
		okStrIdx++;
	else if ( data == ring[ 0 ] )
		ringStrIdx++;

}

status getStatus( void )
{
	return moduleStatus;
}

void moduleOn( void )
{
	ONOFF_LOW;
	delay_ms(250);
	ONOFF_HIGH;
	delay_ms(685);
	ONOFF_LOW;
}

void moduleReset( void )
{
	RESET_HIGH;
	delay_ms( 10 );
	RESET_LOW;
}

void modulePowerOff( void )
{
	POWEREN_HIGH;
	delay_ms( 500 );
	POWEREN_LOW;
}

int moduleExecuteCommand( char *cmd, char *resp )
{
	if ( moduleSendCommand( cmd ) == OK )
	{
		if ( bufferSearch( uartGetRxBuffer(), resp ) )
			return TRUE;
	}

	return FALSE;
}

int moduleSendCommand( char  *cmd )
{

	unsigned int rc;

	clearTxBuffer();
	clearRxBuffer();

	rc = NULL;
	moduleAnswer = NULL;

	if ( writeTxBuffer( "AT" ) && writeTxBuffer( cmd ) && writeTxBuffer( "\r" ) )
	{
		flushTxBuffer();
		setTimeout( 1000 );

		while ( !timedOut() & !moduleAnswer );

		if ( timedOut() )
			rc = TIMEOUT;
		else
			rc = moduleAnswer;
	}

	moduleAnswer = 0;

	return rc;
}
