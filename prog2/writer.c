////////////////////////////////////////////////////////////////////////////////
// Main File:	main.c
// This File:	writer.c
// Other Files:	main.c, queue.c, reader.c, munch1.c, munch2.c
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
#include "main.h"

void *write(void *queues);

/*
 * Write the line to standard output
 */
void *write(void *queues){
	Args *args = (Args*)(queues);
	//Get correct queue from args
	Queue *q = args->queues[2];
	//Get first string from queue
	char *buffer = DequeueString(q);

	//Keep printing as long as NULL is not pulled from buffer
	while(buffer != NULL){
		//Write string to stdout
		printf("%s", buffer);
		//Free memory
		free(buffer);
		//Get next string from queue
		buffer = DequeueString(q);
	}
	pthread_exit(NULL);
}
