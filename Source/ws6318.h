/*
 * ws6318.h
 *
 *  Created on: 10.03.2013
 *      Author: al
 */

#ifndef WS6318_H_
#define WS6318_H_

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

typedef enum status_t {
	OFF,
	ON,
	COMMUNICATE,
	SIM_OK,
	REG_OK
} status;

int moduleInit( void );
void moduleOn( void );
void moduleReset( void );
void modulePowerOff( void );
status getStatus( void );
int moduleSendCommand( char  *cmd );
int moduleExecuteCommand( char *cmd, char *resp );
void moduleUartActivity( char data );
int moduleConfigureGPIO( unsigned char io, unsigned char dir, unsigned char pull );
int moduleSetGPIO( unsigned char io, unsigned char val );

#endif /* WS6318_H_ */
