/*
 * util.c
 *
 *  Created on: 23.04.2013
 *      Author: al
 */

#include "string.h"

void reverse( char str[] );

unsigned int strlen1( char *str )
{
	unsigned int i = 0;

	while ( str[ i++ ] );

	return i;
}

void itoa( int n, char *str )
{
	int i, sign;

	sign = n;

	if ( sign < 0 )
		n = -n;

	i = 0;
	do
	{
		str[ i++ ] = n % 10 + 0x30;
	} while ( (n /= 10) >= 1 );

	if ( sign < 0 )
		str[ i++ ] = '-';

	str[ i ] = '\0';

	reverse( str );
}

void reverse( char *str )
{
	int i,j;
	char c;

	for ( i = 0, j = strlen( str ) - 1; i < j; i++, j-- )
	{
		c = str[ i ];
		str[ i ] = str[ j ];
		str[ j ] = c;
	}

}
