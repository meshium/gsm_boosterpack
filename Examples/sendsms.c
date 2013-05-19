/*
 * sendsms.c
 *
 *  Created on: 24.04.2013
 *      Author: al
 */


#include "msp430g2553.h"
#include "interrupts.h"
#include "string.h"
#include "global.h"
#include "util.h"
#include "gsm.h"

void setup( void );
unsigned int getTemperature( void );

static volatile int btnPressed = FALSE;
static volatile int tempConversionDone = FALSE;

int main( void )
{
	char temp[] = { "000" };

	unsigned int t;

	setup();					// setting up ports and registers

	initUart();

	__enable_interrupt();

	if ( gsmInit() )			// initializing GSM module (powering up, checking SIM-card)
	{

		while ( 1 )
		{
			while ( !btnPressed );

			t = (( ( getTemperature() - 680) * 423) / 1024);
			itoa( t, temp );

			gsmCreateSMS( "+00000000000" );   // <====  Insert the recipient phone number here

			sendData( "GSM Boosterpack on the wire. The temperature around is " );
			sendData( temp );
			sendData( "oC degree\r\n" );

			gsmSendSMS();

			btnPressed = FALSE;
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

	P1DIR &= ~BTN;							// P1.3 input
	P1OUT |=  BTN;							// P1.3 set
	P1REN |= BTN;							// P1.3 pullup
	P1IE |= BTN;							// P1.3 interrupt enabled
	P1IES |= BTN;							// P1.3 Hi/lo edge
	P1IFG &= ~BTN;							// P1.3 IFG cleared
}

unsigned int getTemperature( void )
{

	ADC10CTL0 &= ~ENC;							// Disable ADC
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE;	// Use reference,
													//   16 clock ticks, internal reference on
													//   ADC On, enable ADC interrupt, Internal  = 'ref'

	ADC10CTL1 = ADC10SSEL_3 + INCH_10;		// Set TEMP channel, SMCLK
	__delay_cycles ( 256 );					// Delay to allow Ref to settle
	ADC10CTL0 |= ENC + ADC10SC; 			// Enable and start conversion

    while(ADC10CTL1 & BUSY);

    return ADC10MEM;
}


#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
	tempConversionDone = TRUE;
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	btnPressed = TRUE;
	P1IFG &= ~BTN;
}
