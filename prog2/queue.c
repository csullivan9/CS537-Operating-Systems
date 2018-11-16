////////////////////////////////////////////////////////////////////////////////
// Main File:	main.c
// This File:	queue.c
// Other Files:	main.c, reader.c, munch1.c, munch2.c, writer.c
// 
// Authors:		Deryk Clary 	& 	Chris Sullivan
// NetIDs:		dclary			&	csullivan9
// CS Logins:	deryk			&	csullivan
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

//defining QUEUE_SIZE as const gave scoping error
#define QUEUE_SIZE 10

typedef struct queue Queue;
Queue *CreateStringQueue(int size);
void   EnqueueString(Queue *q, char *string);
char  *DequeueString(Queue *q);
void   PrintQueueStats(Queue *q);

struct queue {
	char *strings[QUEUE_SIZE];			//Holds pointers to strings 
	int   enqueueCount;			//Number of enqueues
	int   dequeueCount;			//Number of dequeues
	int   enqueueBlockCount;	//Number of times enqueue got blocked
	int   dequeueBlockCount;	//Number of times dequeue got blocked
	int   num_enqueued;			//Number of strings currently queued
	sem_t mutex, full, empty;	//Semaphores to handle synchronization
};

/* 
 * places pointer to the string at the end of queue q.
 * 
 * param q         pointer to queue
 * param string    pointer to string that is to be placed in q
 */
void EnqueueString(Queue *q, char *string) {
	//check if queue is full, increment block count if true
	if(q->num_enqueued == QUEUE_SIZE) {
		q->enqueueBlockCount++;
	}

	sem_wait(&(q->full));	//Block if queue full
	sem_wait(&(q->mutex));	//Block if another thread modifying queue

	q->strings[q->num_enqueued] = string; //Put string in queue
	q->enqueueCount++;		//Increment total num of queues
	q->num_enqueued++;		//Increment curr num of strings in queue

	sem_post(&(q->empty));	//sem_v empty since another spot is taken
	sem_post(&(q->mutex));	//Release mutex
}

/*
 * removes a pointer to a string from beginning of queue q
 *
 * param q    pointer to queue
 */
char *DequeueString(Queue *q) {
	//check if queue is empty, increment count if true
	if(q->num_enqueued == 0) {
		q->dequeueBlockCount++;
	}

	sem_wait(&(q->empty));	//Block if queue empty
	sem_wait(&(q->mutex));	//Block if another thread modifying queue

	char* dequeue = q->strings[0]; //Store first string in queue to return later

	//Shift all other pointers one spot to the left
	for(int i = 0; i < q->num_enqueued; i++) {
		q->strings[i] = q->strings[i + 1];
	}
	q->dequeueCount++;		//Increment total num of dequeues
	q->num_enqueued--;		//Decrement curr num of strings in queue

	sem_post(&(q->full));	//sem_v full since a spot is freed in queue
	sem_post(&(q->mutex));	//Release mutex
	return dequeue;			//Return dequeued string
}

/*
 * prints statistics for queue
 *
 * param q    pointer to queue
 */
void PrintQueueStats(Queue *q) {
	fprintf(stderr, "Current number of strings enqueued: %i\n", q->enqueueCount - 1);
	fprintf(stderr, "Current number of strings dequeued: %i\n", q->dequeueCount - 1);
	fprintf(stderr, "Number of threads blocked on enqueue: %i\n", q->enqueueBlockCount);
	fprintf(stderr, "Number of threads blocked on dequeue: %i\n", q->dequeueBlockCount);
}

/* 
 * dynamically allocates a new Queue structure and initializes with array of char 
 * pointers of length size and initialized other data members. 
 *
 * param size	size of the string array that is to be initialized in queue
 * returns	pointer to newly allocated queue
 */
Queue *CreateStringQueue(int size) {
	//initialize queue data members
	Queue *new_queue;
	if((new_queue = malloc(sizeof(Queue))) == NULL) {
		printf("failed to allocate mem for queue");
		return 0;
	}
	new_queue->enqueueCount = 0;
	new_queue->dequeueCount = 0;
	new_queue->enqueueBlockCount = 0;
	new_queue->dequeueBlockCount = 0;
	new_queue->num_enqueued = 0;

	//initialize semaphores for critical sections and full/empty in enqueue and dequeue
	sem_init(&(new_queue->mutex), 0, 1);
	sem_init(&(new_queue->full), 0, 10);
	sem_init(&(new_queue->empty), 0, 0);
	//check that malloc returned successfully
	if(new_queue->strings == NULL) {
		printf("Failed to dynamically allocate memory for queue.");
		return 0;
	}

	return new_queue;
}
