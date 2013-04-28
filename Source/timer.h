/*
 * delay.h
 *
 *  Created on: 10.03.2013
 *      Author: al
 */

#include "global.h"

#ifndef DELAY_H_
#define DELAY_H_

void tick1ms( void );
void initTimer( void );
void delay_ms( unsigned int d );
void setTimeout( unsigned int t );
int timedOut( void );
unsigned long getTimestamp( void );

#endif /* DELAY_H_ */
