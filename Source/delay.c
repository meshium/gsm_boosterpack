/*
 * delay.c
 *
 *  Created on: 10.03.2013
 *      Author: al
 */

#include "msp430g2553.h"
#include "delay.h"

typedef void( *voidFuncPtr )( void );
static volatile voidFuncPtr Tick1msFunc;

volatile unsigned int ms;
volatile int timeout_flag;

void delay_ms( unsigned int d )
{
	TACCR0 += 16000 - 1;
	ms = d;
	while( ms );
}

void setTimeout( unsigned int t )
{
	timeout_flag = FALSE;
	TACCR0 += 16000 - 1;
	ms = t;
}

int timedOut( void )
{
	return timeout_flag;
}

void initTimerA( void )
{
	TACCR0 = 16000 - 1;
	TACCTL0 = CCIE;
	TACTL = TASSEL_2 + MC_2 + TAIE + TACLR;           // SMCLK, contmode, interrupt enabled

	Tick1msFunc = 0;

}

void attachTimerA( void( *tick_func )( void ) )
{
	Tick1msFunc = tick_func;
}

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)
{
	TACCR0 += 16000 - 1;

	if ( ms )
		ms--;
	else
		timeout_flag = TRUE;

	if ( Tick1msFunc )
		Tick1msFunc();

}

// Timer A1 Interrupt Vector (TA0IV) handler
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A1(void)
{
	switch( TA0IV )
	{
	case 2:
		break;
	case 10:
		break;
	}

}
