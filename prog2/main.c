////////////////////////////////////////////////////////////////////////////////
// Main File:	main.c
// This File:	main.c
// Other Files:	queue.c, reader.c, munch1.c, munch2.c, writer.c
// 
// Authors:		Deryk Clary 	& 	Chris Sullivan
// NetIDs:		dclary			&	csullivan9
// CS Logins:	deryk			&	csullivan
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"
#include "reader.h"
#include "munch1.h"
#include "munch2.h"
#include "writer.h"

//typedef struct args Args;
static const int QUEUE_SIZE = 10;
static const int NUM_OF_QUEUES = 3;

/* 
 * Initialize 3 queues, 4 pthreads
 * Call reader to read in strings
 * Call munches
 * Call writer to write to file/cmdline
 */
int main(int argc, char *argv[]) {
	//Initialize queues
	Args *thread_args = malloc(sizeof(Args));
	for(int i = 0; i < NUM_OF_QUEUES; i++) {
		thread_args->queues[i] = CreateStringQueue(QUEUE_SIZE);
	}

	//Initialize pthreads for each thread needed
	pthread_t reader;
	pthread_t munch1;
	pthread_t munch2;
	pthread_t writer;
	
	//Create threads
	if(pthread_create(&reader, NULL, read, (void*) thread_args) != 0) {
		printf("Failed to create reader thread\n");
	}

	if(pthread_create(&munch1, NULL, replace, (void *) thread_args) != 0) {
		printf("Failed to create munch1 thread\n");
	}

	if(pthread_create(&munch2, NULL, toUpper, (void *) thread_args) != 0) {
		printf("Failed to create munch2 thread\n");
	}
	
	if(pthread_create(&writer, NULL, write, (void *) thread_args) != 0) {
		printf("Failed to create writer thread\n");
	}

	//Join threads
	if(pthread_join(reader, NULL) != 0) {
		printf("Error in returning from reader thread.\n");
	}

	if(pthread_join(munch1, NULL) != 0) {
		printf("Error in returning from munch1 thread.\n");
	}

	if(pthread_join(munch2, NULL) != 0) {
		printf("Error in returning from munch2 thread.\n");
	}

	if(pthread_join(writer, NULL) != 0) {
		printf("Error in returning from writer thread.\n");
	}

	//Print stats of each queue
	for(int i = 0; i < NUM_OF_QUEUES; i++) {
		//Print name of queue, the stats, then a blank line to make output more readable
		if(i == 0) fprintf(stderr, "munch1 Queue\n");
		else if(i == 1) fprintf(stderr, "munch2 Queue\n");
		else if(i == 2) fprintf(stderr, "writer Queue\n");

  		PrintQueueStats(thread_args->queues[i]);

		fprintf(stderr, "\n");
	}
}
