/*
 * blink.c
 *
 *  Created on: 10.03.2013
 *      Author: al
 */


#include "msp430g2553.h"
#include "global.h"
#include "gsm.h"

void setup( void );

int main( void )
{
	unsigned char wismoLED = 0;

	setup();					// setting up ports and registers

	if ( gsmInit() )			// initializing GSM module (powering up, checking SIM-card)
	{
			if ( gsmConfigureGPIO( WISMOLED, OUTPUT, PULLDOWN ) )
			{
				while ( 1 )
				{
					delay_ms( 500 );

					wismoLED ^= 0x01;
					gsmSetGPIO( WISMOLED, wismoLED );	// blinking by the GSM boosterpack led
				}

			}
	}

	P1OUT |= LED0;	// turn on red led on the launchpad - module initialisation was unsuccessful

	while (1);
}

// Setting up MCU ports and registers
void setup( void )
{
	WDTCTL = WDTPW + WDTHOLD;	// disabling watchdog timer

	if (CALBC1_1MHZ==0xFF)
	{
		while(1);				// Stop if calibration table erased
	}

	DCOCTL = 0;					// Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ;      // Set DCO 1MHZ
	DCOCTL = CALDCO_1MHZ;

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
