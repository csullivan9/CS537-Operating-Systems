////////////////////////////////////////////////////////////////////////////////
// Main File:	main.c
// This File:	munch2.c
// Other Files:	main.c, queue.c, reader.c, munch1.c, writer.c
// 
// Authors:		Deryk Clary 	& 	Chris Sullivan
// NetIDs:		dclary			&	csullivan9
// CS Logins:	deryk			&	csullivan
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "queue.h"
#include "main.h"

void *toUpper(void *queues);

/*
 * Changes all lower case chars to upper case
 */
void *toUpper(void *queues){
	Args *args = (Args*)(queues);
	//Get queues
	Queue *from = args->queues[1];
	Queue *to	= args->queues[2];

	//Initialize variables
	char *curr;								//Holds curr position in the string
	char *newString = DequeueString(from);	//Dequeue first string

	//Begin changing, queueing, and dequeueing strings
	while(newString != NULL){
		//Find & replace lower case
		curr = newString;
		while(*curr != '\n'){
			*curr = toupper(*curr);
			curr++;
		}

		//Queue string for writer
		EnqueueString(to, newString);
		//Dequeue next string
		newString = DequeueString(from);
	}
	EnqueueString(to, NULL); //Put NULL into queue to signal other threads to quit
	pthread_exit(NULL);
} 
