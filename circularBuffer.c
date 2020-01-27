/*
 * circularBuffer.c
 *
 * Created: 10/31/2019 9:17:15 PM
 *  Author: egeozgul
 */ 
#include "circularBuffer.h"

struct QUEUE select = {{0},0,0,0};

uint8_t nextIndex(uint8_t currentIndex,uint8_t nElements)
{
	return (currentIndex+1)%nElements;
}

//adds element
void enqueue(struct QUEUE *queue, uint8_t newData)
{
	if(queue->size == CIRCULAR_BUFFER_SIZE)
	{
		queue->circularBuffer[queue->back] = newData;
		queue->back = nextIndex(queue->back,CIRCULAR_BUFFER_SIZE);
		queue->front = nextIndex(queue->front,CIRCULAR_BUFFER_SIZE);
		return;
	}
	
	queue->circularBuffer[queue->back] = newData;
	queue->back = nextIndex(queue->back,CIRCULAR_BUFFER_SIZE);
	queue->size++;
}

//removes and return the last element added
uint8_t dequeue(struct QUEUE *queue)
{
	if(queue->size == 0)
		return 0;
	
	uint8_t dequedElement = queue->circularBuffer[queue->front];
	queue->front = nextIndex(queue->front, CIRCULAR_BUFFER_SIZE);
	queue->size--;
	return dequedElement;
}

//returns the element to be dequed first without dequeueing it
uint8_t getLastElement(struct QUEUE *queue)
{
	return queue->circularBuffer[queue->back];
}