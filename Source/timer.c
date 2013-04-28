/*
 * timer.c
 *
 *  Created on: 10.03.2013
 *      Author: al
 */

#include "msp430g2553.h"
#include "global.h"
#include "interrupts.h"
#include "timer.h"

static volatile voidFuncPtr Tick1msFunc;

volatile unsigned long timestamp;
volatile unsigned long alarm;

void tick1ms( void );

void initTimer( void )
{
	timestamp = 0;
	initTimerA0( TIMER_DIV );
	attachTimerA0( tick1ms );
}

void tick1ms( void )
{
	timestamp++;

}

unsigned long getTimestamp( void )
{
	return timestamp;
}

void delay_ms( unsigned int delay )
{
	unsigned long stop;
	stop = timestamp + delay;
	while( timestamp < stop );
}

void setTimeout( unsigned int t )
{
	alarm = timestamp + t;
}

int timedOut( void )
{
	if ( timestamp > alarm )
		return TRUE;
	else
		return FALSE;
}
