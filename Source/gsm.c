/*
 * gsm.c
 *
 *  Created on: 10.03.2013
 *      Author: al
 */

#include "msp430g2553.h"
#include "global.h"
#include "interrupts.h"
#include "string.h"
#include "gsm.h"
#include "buffer.h"

volatile status moduleStatus;

static volatile int gotRing = FALSE;
static volatile unsigned int gsmCountdown = 0;
static volatile int gsmCountdownStart = FALSE;
static volatile int gsmReceiveTimeout = FALSE;

void gsmTick1ms( void );
void gsmUartRx( char c );
void gsmPowerOn( void );
void gsmPowerOff( void );
void gsmReset( void );

const char ok[] = { "OK\r\n" };
const char simOK[] = { "\r\n+CPIN: READY\r\n\r\nOK\r\n" };
const char networkOK[] = { "\r\n+CREG: 0,1\r\n\r\nOK\r\n" };
const char error[] = { "ERROR\r\n" };
const char ring[] = { "RING\r\n" };
const char prompt[] = { "\r\n>" };

int gsmInit( void )
{
	int i;

	initTimer();
	initUart();

	attachTimerA0( gsmTick1ms );
	attachUsciRx( gsmUartRx );

	__enable_interrupt();

	for ( i=0; i < INIT_TRIES; i++ )
	{

		if ( ~READY_PORT & READY_PIN )
			gsmReset();
		else
			gsmPowerOn();

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
			if ( gsmExecuteCommand( "E0", ok, CMD_TIMEOUT ) )
			{
				moduleStatus = COMMUNICATE;

				if ( gsmExecuteCommand( "+CPIN?", simOK, CMD_TIMEOUT ) )
				{
					moduleStatus = SIM_OK;

					if ( gsmExecuteCommand( "+CMGF=1", ok, CMD_TIMEOUT ) )
					{
						moduleStatus = SMS_OK;

						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;

}

void gsmTick1ms( void )
{

	if (  gsmCountdownStart )
	{
		if ( gsmCountdown )
			gsmCountdown--;
		else
		{
			gsmCountdownStart = FALSE;
			gsmReceiveTimeout = TRUE;
		}
	}

	tick1ms();
}

void gsmUartRx( char c )
{
	gsmCountdown = BREAK_TIMEOUT;
	gsmCountdownStart = TRUE;
	gsmReceiveTimeout = FALSE;

	uartBufferedRx( c );
}

int gsmConfigureGPIO( unsigned char io, unsigned char dir, unsigned char pull )
{

	char cmd[] = { "+WMGPIOCFG=00,0,0" };

	if ( io >= 1 && io <= 12 && dir < 2 && pull < 2 )
	{
		cmd[11] = io / 10 + 0x30;
		cmd[12] = io % 10 + 0x30;
		cmd[14] = dir + 0x30;
		cmd[16] = pull + 0x30;

		if ( gsmExecuteCommand( cmd, ok, CMD_TIMEOUT ) )
			return TRUE;
	}

	return FALSE;
}

int gsmSetGPIO( unsigned char io, unsigned char val )
{
	char cmd[] = { "+WMGPIO=00,0" };

	if ( io >= 1 && io <= 12 && val < 2 )
	{
		cmd[8] = io / 10 + 0x30;
		cmd[9] = io % 10 + 0x30;
		cmd[11] = val + 0x30;

		if ( gsmExecuteCommand( cmd, ok, CMD_TIMEOUT ) )
			return TRUE;
	}

	return FALSE;
}

status gsmGetStatus( void )
{
	return moduleStatus;
}

void gsmPowerOn( void )
{
	ONOFF_LOW;
	delay_ms(250);
	ONOFF_HIGH;
	delay_ms(685);
	ONOFF_LOW;
}

void gsmReset( void )
{
	RESET_HIGH;
	delay_ms( 10 );
	RESET_LOW;
}

void gsmPowerOff( void )
{
	POWEREN_HIGH;
	delay_ms( 500 );
	POWEREN_LOW;
}

int gsmExecuteCommand( const char  *cmd, const char *resp, unsigned int time2answer )
{

	clearTxBuffer();
	clearRxBuffer();

	if ( writeTxBuffer( "AT" ) && writeTxBuffer( cmd ) && writeTxBuffer( "\r" ) )
	{
		flushTxBuffer();

		while ( !txBufferEmpty() );

		setTimeout( time2answer );
		gsmReceiveTimeout = FALSE;

		while ( !gsmReceiveTimeout && !timedOut() );

		if ( !rxBufferEmpty() && rxBufferOverflow() == 0 )
		{
			if ( bufferSearch( uartGetRxBuffer(), resp ) )
				return TRUE;
		}
	}

	return FALSE;
}
