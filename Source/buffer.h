/*
 * buffer.h
 *
 *  Created on: 01.04.2013
 *      Author: al
 */

#include "global.h"

#ifndef BUFFER_H_
#define BUFFER_H_

#define CRITICAL_SECTION_START	__disable_interrupt();
#define CRITICAL_SECTION_END	__enable_interrupt();

typedef struct cBuffer_t {
	char *data_ptr;
	unsigned int size;
	unsigned int data_idx;
	unsigned int data_len;
} cBuffer;

void bufferInit( cBuffer *buffer, char *start, unsigned int size );
int bufferRead( cBuffer *buffer, char *data );
int bufferWrite( cBuffer *buffer, char data );
int bufferSearch( cBuffer *buffer, const char *data );
void bufferFlush( cBuffer *buffer );
int bufferEmpty( cBuffer *buffer );

#endif /* BUFFER_H_ */
