#ifndef __537malloc_h__
#define __537malloc_h__
#include <stdbool.h>
typedef struct block{
	void * 	ptr;
	size_t	size;
	bool	freed;
} block;

void *malloc537(size_t size);
void free537(void *ptr);
void *realloc537(void *ptr, size_t size);
void memcheck537(void *ptr, size_t size);
block *block_init(void *ptr, size_t size);
#endif // __537malloc_h__
