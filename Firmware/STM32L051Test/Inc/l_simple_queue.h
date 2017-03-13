#ifndef L_SIMPLE_QUEUE_H
#define L_SIMPLE_QUEUE_H

#include "stm32l0xx.h"

typedef struct squeue{
	uint8_t length;
	uint8_t cursor;
	uint8_t unitSize;
	uint8_t* content;
} squeue_t;

// squeue_t squeue_create();
#define  _m_squeue_create(TYPE,NAME,LENGTH) \
		uint8_t NAME##_content[sizeof(TYPE)*(LENGTH)]; \
		squeue_t NAME = { LENGTH, 0, sizeof(TYPE), NAME##_content}; 

void squeue_append(squeue_t* q, uint8_t* datap);
void squeue_insert(squeue_t* q, uint8_t* datap, int position);

uint8_t* squeue_drop(squeue_t* q);
uint8_t* squeue_peek(squeue_t* q, int position);
		
void squeue_remove(squeue_t* q, int position);
		
#endif /* L_SIMPLE_QUEUE_H*/
