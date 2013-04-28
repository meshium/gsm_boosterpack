/*
 * gsm.h
 *
 *  Created on: 10.03.2013
 *      Author: al
 */

#ifndef WS6318_H_
#define WS6318_H_

#include "uart.h"
#include "timer.h"

#define ONOFF_PIN BIT5
#define ONOFF_PORT P1OUT

#define RESET_PIN BIT4
#define RESET_PORT P1OUT

#define POWEREN_PIN BIT7
#define POWEREN_PORT P1OUT

#define READY_PIN BIT5
#define READY_PORT P2IN

#define ONOFF_LOW ONOFF_PORT &= ~ONOFF_PIN
#define ONOFF_HIGH ONOFF_PORT |= ONOFF_PIN

#define RESET_LOW RESET_PORT &= ~RESET_PIN
#define RESET_HIGH RESET_PORT |= RESET_PIN

#define POWEREN_LOW POWEREN_PORT &= ~POWEREN_PIN
#define POWEREN_HIGH POWEREN_PORT |= POWEREN_PIN

#define NULL 0
#define OK 1
#define ERROR 2
#define TIMEOUT 3

#define BREAK_TIMEOUT 10	// timed out if line idle 20ms
#define CMD_TIMEOUT 200

typedef enum status_t {
	OFF,
	ON,
	COMMUNICATE,
	SIM_OK,
	REG_OK,
	SMS_OK
} status;

int gsmInit( void );
int gsmConfigureGPIO( unsigned char io, unsigned char dir, unsigned char pull );
int gsmSetGPIO( unsigned char io, unsigned char val );
status gsmGetStatus( void );
int gsmExecuteCommand( const char  *cmd, const char *resp, unsigned int time2answer );

#endif /* WS6318_H_ */
