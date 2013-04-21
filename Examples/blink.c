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

void setup()
{
	if (CALBC1_16MHZ==0xFF)
	{
		while(1);
	}

	DCOCTL = 0;
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL = CALBC1_16MHZ;

	P1SEL = 0;
	P1SEL2 = 0;

	P1DIR |= (LED0 + LED1 + ONOFF_PIN + RESET_PIN + POWEREN_PIN );
	P1OUT &= ~(LED0 + LED1 + ONOFF_PIN + RESET_PIN + POWEREN_PIN );

	P2DIR &= ~READY_PIN;
	P2REN |= READY_PIN;
	P2OUT |= READY_PIN;

}

void moduleBlinkLED()
{
	if ( tickCount++ >= 500 )
	{
		tickCount = 0;
		wismoBlink = TRUE;
	}
}

int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;

	setup();
	initTimerA();
	initUart();

	__enable_interrupt();

	unsigned char wismoLED = 0;
	wismoBlink = FALSE;

	if ( moduleInit() )
	{
			if ( moduleConfigureGPIO( WISMOLED, OUTPUT, PULLDOWN ) )
			{
				attachTimerA( moduleBlinkLED );

				while ( 1 )
				{
					while( ~wismoBlink );

					wismoBlink = FALSE;
					wismoLED ^= 0x01;
					moduleSetGPIO( WISMOLED, wismoLED );
				}

			}
	}

	P1OUT |= LED0;

	while (1);
}
