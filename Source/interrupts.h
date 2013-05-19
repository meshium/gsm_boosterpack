/*
 * interrupts.h
 *
 *  Created on: 25.04.2013
 *      Author: al
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#define RX BIT1
#define TX BIT2

#define CLI_USCITX IE2 &= ~UCA0TXIE;
#define STI_USCITX IE2 |= UCA0TXIE;
#define CLI_USCIRX IE2 &= ~UCA0RXIE;
#define STI_USCIRX IE2 |= UCA0RXIE;
#define CLI_TA0	 TACTL &= ~TAIE;
#define STI_TA0	 TACTL |= TAIE;
#define CLI_ADC10 ADC10CTL0 &= ~ADC10IE;
#define STI_ADC10 ADC10CTL0 |= ADC10IE;
#define CLI_PORT1 P1IE &= ~BTN;			//  !!!
#define STI_PORT1 P1IE |= BTN;			//  !!!

#define ADC10START ADC10CTL0 |= ENC + ADC10SC;

typedef int( *intFuncPtr )( char *c );
typedef void( *voidFuncPtr )( char c );
typedef void( *voidFuncIntPtr )( int i );
typedef void( *voidFuncVoidPtr )( void );

//void initPORT1( void );
//void initADC10( void );
void initTimerA0( unsigned int div );
void initUsci2Uart( void );
void attachUsciRx( void( *rx_func )( char c ) );
void attachUsciTx( int( *tx_func )( char *c) );
void attachTimerA0( void( *tick_func )( void ) );
//void attachADC10( void( *adc10_func )( int ) );
//void attachPORT1( void( *p1_func )( void ) );

#endif /* INTERRUPTS_H_ */
