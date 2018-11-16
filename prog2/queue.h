#ifndef __queue_h__
#define __queue_h__

typedef struct queue Queue;
Queue *CreateStringQueue(int size);
void EnqueueString(Queue *q, char *string);
char *DequeueString(Queue *q);
void PrintQueueStats(Queue *q);
#endif //__queue_h__

