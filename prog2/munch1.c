////////////////////////////////////////////////////////////////////////////////
// Main File:	main.c
// This File:	munch1.c
// Other Files:	main.c, queue.c, reader.c, munch2.c, writer.c
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

void *replace(void *queues);

/*
 * Replaces all spaces in string with *
 *
 * Args:
 * queues: Structure containing pointers to all of the queues
 */
void *replace(void *queues){
	Args  *args = (Args*)(queues);
	Queue *from = args->queues[0]; //Dequeue from reader
	Queue *to	= args->queues[1]; //Queue strings to munch2

	//Initialize variables
	char *space;							//Holds occurrences of spaces in s
	char *last_space;						//Holds last occurrence of space in s
	char *newString = DequeueString(from);	//Change s into char array

	//Once dequeue returns NULL, it's time to exit
	while(newString != NULL){
		//Find & replace spaces with *
		space      = strchr(newString, ' ');	//First space occurrence
		last_space = strrchr(newString, ' ');	//Last space occurrence
		while(space != last_space){
			*space = '*';
			space = strchr(space, ' ');
		}
		//Check if last_space NULL to prevent seg fault on empty files/lines
		if(last_space != NULL) *last_space = '*';

		//Queue changed string
		EnqueueString(to, newString);
		//Dequeue new string
		newString = DequeueString(from);
	}
	EnqueueString(to, NULL); //Put NULL into queue to signal other threads to quit
	pthread_exit(NULL);
}
