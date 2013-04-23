/*
 * util.c
 *
 *  Created on: 23.04.2013
 *      Author: al
 */

unsigned int strlen( char *str )
{
	unsigned int i = 0;

	while ( str[ i++ ] );

	return i;
}
