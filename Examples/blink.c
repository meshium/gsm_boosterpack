/*
 * main.c
 *
 *  Created on: 10.03.2013
 *      Author: al
 */


#include "msp430g2553.h"
#include "global.h"
#include "ws6318.h"
#include "uart.h"
#include "delay.h"

volatile unsigned int tickCount;
volatile int wismoBlink;

// Setting up MCU ports and registers
void setup( void )
{
	if (CALBC1_16MHZ==0xFF)
	{
		while(1);				// Stop if calibration table erased
	}

	DCOCTL = 0;					// Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_16MHZ;		// Set DCO to 16MHz
	DCOCTL = CALBC1_16MHZ;		//

	P1SEL = 0;
	P1SEL2 = 0;

	// Configuring pins to output mode
	// LED0, LED1 - red and green leds on launchpad board
	// ONOFF_PIN - GSM module turning on/off control pin
	// RESET_PIN - GSM module hardware resetting pin
	// POWEREN_PIN - pin for disabling onboard GSM boosterpack DC/DC
	P1DIR |= (LED0 + LED1 + ONOFF_PIN + RESET_PIN + POWEREN_PIN );
	P1OUT &= ~(LED0 + LED1 + ONOFF_PIN + RESET_PIN + POWEREN_PIN );

	// Configuring pin to input mode
	// READY_PIN - signaling about successfully powering GSM module on
	P2DIR &= ~READY_PIN;
	P2REN |= READY_PIN;
	P2OUT |= READY_PIN;

}

// Receive control every 1ms from TimerA interrupt handler
// Every 500 cycles (0.5 sec) sets the flag to turning on/off GSM module GPIO
// that connected to the GSM boosterpack LED indicator (WISMO_LED)
void moduleBlinkLED( void )
{
	if ( tickCount++ >= 500 )
	{
		tickCount = 0;
		wismoBlink = TRUE;
	}
}

int main( void )
{
	WDTCTL = WDTPW + WDTHOLD;	// disabling watchdog timer

	setup();					// setting up ports and registers
	initTimerA();				// initializing TimerA
	initUart();					// initializing USCIA0 to 9600 baudrate

	__enable_interrupt();

	unsigned char wismoLED = 0;
	wismoBlink = FALSE;

	if ( moduleInit() )			// initializing GSM module (powering up, checking SIM-card)
	{
			if ( moduleConfigureGPIO( WISMOLED, OUTPUT, PULLDOWN ) )
			{
				attachTimerA( moduleBlinkLED );	// attach function to handling 1ms ticks

				while ( 1 )
				{
					while( ~wismoBlink );

					wismoBlink = FALSE;
					wismoLED ^= 0x01;
					moduleSetGPIO( WISMOLED, wismoLED );	// blinking by the GSM boosterpack led
				}

			}
	}

	P1OUT |= LED0;	// turn on red led on the launchpad - module initialisation was unsuccessful

	while (1);
}
