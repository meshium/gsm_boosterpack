/*
 * delay.h
 *
 *  Created on: 10.03.2013
 *      Author: al
 */

#include "global.h"

#ifndef DELAY_H_
#define DELAY_H_

void initTimerA( void );
void delay_ms( unsigned int d );
void setTimeout( unsigned int t );
int timedOut( void );
void attachTimerA( void( *tick_func )( void ) );

#endif /* DELAY_H_ */
