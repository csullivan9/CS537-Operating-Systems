////////////////////////////////////////////////////////////////////////////////
// Main File:	main.c
// This File:	reader.c
// Other Files:	main.c, queue.c, munch1.c, munch2.c, writer.c
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

struct Args;
void *read(void *queues);

const int BUFFER_SIZE = 1024;

/*
 * Read lines from standard input, queue each of them for Munch1
 * Oversized lines will be flushed, never to be seen again
 * 
 * Args:
 * q: This is the queue for Munch1
 */
void *read(void *queues){
	Args *args = (Args*)(queues);
	//Initialize variables
	Queue *q = args->queues[0];	//Queue to put strings in from input
	char  *buffer;				//Buffer
	char  *curr;				//Holds curr spot in buffer for writing
	char  c;					//Holds curr char
	int   to_queue; 			//Tells whether buffer should be queued

	//Read each line from input as string and queue them for munch1
	c = fgetc(stdin);
	while(c != EOF){
		//Allocate memory for the next string
		buffer = malloc(BUFFER_SIZE);

		//Check malloc return value. NULL (false) indicates failure.
		if(buffer == NULL){
			printf("Buffer malloc failed.\n");
			exit(1);
		}

		//Reset variables
		to_queue = 1;
		curr     = buffer;	//Points to beginning of buffer

		//Scan line of input
		for(int i = 0; i < BUFFER_SIZE; i++){
			//EOF -> break to end while loop, do not queue
			if(c == EOF) {
				printf("End of file: %s\n", buffer);
				if(curr != buffer) *curr = '\n';
				//to_queue = 0;
				break;
			}

			//End of input
			else if(c == '\0'){
				*curr = '\n';
				curr++;
				break; //Stop loop and continue to queue
			}

			//End of line
			else if(c == '\n'){
				*curr = '\n';
				curr++;
				c = fgetc(stdin);
				break; //Stop loop and continue to queue
			}

			//Line too big for buffer -> flush down toilet
			if(i == BUFFER_SIZE-1) {
				printf("Input line too big for buffer\n");
				//Flush
				while(c != '\n' && c != '\0') {
					c = fgetc(stdin);
				}
				to_queue = 0; //Set to 0 so the line does not get queue
				c = fgetc(stdin);
				break; //Stop loop, don't queue
			}

			//Put char in buffer
			*curr = c;
			curr++;

			//Get next char
			c = fgetc(stdin);
		}
		
		//Queue buffer for munch1, if it did not overflow and not EOF
		if(to_queue == 1) {
			EnqueueString(q, buffer);
		}
	}
	EnqueueString(q, NULL); //Put NULL into queue to signal other threads to quit
	pthread_exit(NULL);
}
