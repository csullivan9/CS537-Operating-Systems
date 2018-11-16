#ifndef __main_h__
#define __main_h__

//typedef struct args Args;
//Struct to be passed as arg in pthread_create
typedef struct args{
	Queue *queues[3];
} Args;
int main(int argc, char *argv[]);

#endif //__main_h__

