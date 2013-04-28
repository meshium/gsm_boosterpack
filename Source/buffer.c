/*
 * buffer.c
 *
 *  Created on: 01.04.2013
 *      Author: al
 */

#include "msp430g2553.h"
#include "buffer.h"
#include "string.h"

void bufferInit( cBuffer *buffer, char *start, unsigned int size )
{
	buffer->data_ptr = start;
	buffer->size = size;
	buffer->data_idx = 0;
	buffer->data_len = 0;
}

int bufferRead( cBuffer *buffer, char *data )
{
	*data = 0;

	if ( buffer->data_len )
	{
		*data = buffer->data_ptr[ buffer->data_idx ];
		buffer->data_idx++;

		if ( buffer->data_idx >= buffer->size )
			buffer->data_idx -= buffer->size;

		buffer->data_len--;

		return TRUE;

	}
	else
		return FALSE;

}

int bufferWrite( cBuffer *buffer, char data )
{
	if ( buffer->data_len < buffer->size )
	{
		buffer->data_ptr[ ( buffer->data_idx + buffer->data_len ) % buffer->size ] = data;
		buffer->data_len++;

		return TRUE;
	}

	return FALSE;
}

int bufferSearch( cBuffer *buffer, const char *data )
{
	unsigned int i, j, l;
	int start;

	start = FALSE;
	l = strlen( data );

	for ( i=0; i <= buffer->data_len - l; i++ )
	{
		if ( buffer->data_ptr[ ( buffer->data_idx + i ) % buffer->size ] == data[ 0 ] )
		{
			start = TRUE;
			for ( j = 1; j <  l && start; j++ )
				if ( buffer->data_ptr[ ( buffer->data_idx + i + j ) % buffer->size ] != data[ j ] )
					start = FALSE;

			if ( start )
				return TRUE;

		}
	}

	return FALSE;
}

void bufferFlush( cBuffer *buffer )
{
	buffer->data_len = 0;

}

int bufferEmpty( cBuffer *buffer )
{
	if ( buffer->data_len )
		return FALSE;
	else
		return TRUE;
}
