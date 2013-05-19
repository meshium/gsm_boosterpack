/*
 * interrupts.c
 *
 *  Created on: 24.04.2013
 *      Author: al
 */

#include "msp430g2253.h"
#include "global.h"
#include "interrupts.h"

static volatile intFuncPtr USCITxFunc = 0;
static volatile voidFuncPtr USCIRxFunc = 0;
static volatile voidFuncVoidPtr TickFunc = 0;
static volatile voidFuncIntPtr ADC10Func = 0;
static volatile voidFuncVoidPtr Port1Func = 0;
static volatile unsigned int timerDiv;

void initTimerA0( unsigned int div )
{
	TickFunc = 0;
	timerDiv = div;

	TACCR0 = timerDiv;
	TACCTL0 = CCIE;
	TACTL = TASSEL_2 + MC_2 + TAIE + TACLR;           // SMCLK, contmode, interrupt enabled
}

void initUsci2Uart( void )
{
	P1SEL |= RX + TX;				// USCIA0
	P1SEL2 |= RX + TX;				// P1.1 - Rxd, P1.2 - Txd

//	UCA0CTL1 |= UCSSEL_2;			// SMCLK
//	UCA0BR0 = 104;					// 16MHz 9600
//	UCA0BR1 = 0;					//
//	UCA0MCTL = UCBRF_3 + UCOS16;	//
//	UCA0CTL1 &= ~UCSWRST;			// Initialize USCI state machine

	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	UCA0BR0 = 104;                            // 1MHz 9600
	UCA0BR1 = 0;                              // 1MHz 9600
	UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

	USCIRxFunc = 0;
	USCITxFunc = 0;

	IE2 |= UCA0RXIE;				// Enable USCIA0 rx interrupt

}

void attachUsciRx( void( *rx_func )( char c ) )
{
	CLI_USCIRX;
	USCIRxFunc = rx_func;
	STI_USCIRX;
}

void attachUsciTx( int( *tx_func )( char *c) )
{
	CLI_USCITX;
	USCITxFunc = tx_func;
	STI_USCITX;
}

void attachTimerA0( void( *tick_func )( void ) )
{
	CLI_TA0;
	TickFunc = tick_func;
	STI_TA0;
}

/*
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR()
{
	if ( Port1Func ) Port1Func();
	P1IFG &= ~BTN;
}

#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR()
{
	return;
}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR()
{
	if ( ADC10Func ) ADC10Func( ADC10MEM );
}
*/

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR( void )
{

	char data;

	if ( USCITxFunc )
	{

		if ( USCITxFunc( &data ) )
				UCA0TXBUF = data;
		else CLI_USCITX;
	}

}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR( void )
{

//	char data;

//	data = UCA0RXBUF;

	if ( USCIRxFunc ) USCIRxFunc( UCA0RXBUF );

}

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR (void)
{
	TACCR0 += timerDiv;

	if ( TickFunc ) TickFunc();

}

// Timer A1 Interrupt Vector (TA0IV) handler
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A1_ISR(void)
{
	switch( TA0IV )
	{
	case 2:
		break;
	case 10:
		break;
	}
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR()
{
	return;
}

#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR()
{
	return;
}

#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR()
{
	return;
}

#pragma vector=COMPARATORA_VECTOR
__interrupt void COMPARATORA_ISR()
{
	return;
}

#pragma vector=NMI_VECTOR
__interrupt void NMI_ISR()
{
	return;
}
