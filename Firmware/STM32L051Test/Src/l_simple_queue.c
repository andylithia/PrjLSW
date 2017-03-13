#include "l_simple_queue.h"

/*
typedef struct squeue{
	uint8_t length;
	uint8_t unitSize;
	uint8_t* content;
} squeue_t;

// squeue_t squeue_create();
#define  _m_squeue_create(TYPE,NAME,LENGTH) \
		uint8_t NAME##_content[sizeof(TYPE)*(LENGTH)]; \
		squeue_t NAME## = { LENGTH, sizeof(TYPE), NAME##_content}; 
*/


void squeue_append(squeue_t* q, uint8_t* datap){
	if (q->cursor>=q->length)
		return;
	for(int i=0; i<(q->unitSize);i++)
		q->content[q->cursor*q->unitSize+i]=datap[i];
	q->cursor++;
}

// not implmentated for now
void squeue_insert(squeue_t* q, uint8_t* datap, int position);

uint8_t* squeue_drop(squeue_t* q){
	if(q->cursor>0)
	return (q->content)+(--(q->cursor)*q->unitSize);
	else
	return NULL;
}

uint8_t* squeue_peek(squeue_t* q, int position){
	return q->content+(position*q->unitSize);
}

void squeue_remove(squeue_t* q, int position){
	for(int pos = position; pos<q->length; pos++)
		for(int i=0; i<q->unitSize; i++)
			q->content[pos*q->unitSize+i]=q->content[(pos+1)*q->unitSize+i];
	if(q->cursor>0)
		q->cursor--;
}

