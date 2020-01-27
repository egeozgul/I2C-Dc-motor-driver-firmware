/*
 * circularBuffer.h
 *
 * Created: 10/31/2019 9:17:40 PM
 *  Author: egeozgul
 */ 


#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <avr/io.h>


#ifndef CIRCULAR_BUFFER_SIZE
#define CIRCULAR_BUFFER_SIZE 10
#endif

struct QUEUE
{
	uint8_t circularBuffer[CIRCULAR_BUFFER_SIZE];
	uint8_t front;
	uint8_t back;
	uint8_t size;
};
uint8_t nextIndex(uint8_t currentIndex,uint8_t nElements);

//adds element
void enqueue(struct QUEUE *queue, uint8_t newData);

//removes and return the last element added
uint8_t dequeue(struct QUEUE *queue);

//returns the element to be dequed first without dequeueing it
uint8_t getLastElement(struct QUEUE *queue);



#endif /* CIRCULARBUFFER_H_ */