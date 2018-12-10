////////////////////////////////////////////////////////////////////////////////
// Main File:	537malloc.c
// This File:	537malloc.c
// Other Files:	rb_tree.c
// 
// Authors:		Deryk Clary 	& 	Chris Sullivan
// Emails:		dclary@wisc.edu & 	csullivan@wisc.edu
// NetIDs:		dclary			&	csullivan9
// CS Logins:	deryk			&	csullivan
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rb_tree.h"

void *malloc537(size_t size);
void free537(void *ptr);
void *realloc537(void *ptr, size_t size);
void memcheck537(void *ptr, size_t size);
static void insert_blk(block *to_add);
static void insert_helper(block *value);
static void remove_blk(block *value);
block * block_init(void *newptr, size_t newsize);

static const int CASE1 = 1;
static const int CASE2 = 2;
static const int CASE3 = 3;
static const int CASE4 = 4;

/*
 * Allocate memory of specified size, record tuple of address and length in
 * red-black tree. Must check for size 0
 * 
 * Param:
 * size: Number of bits to allocate
 * Returns: void* to allocated memory
 */
void *malloc537(size_t size){
	//check if tree exists
	if(!tree_check()){
		//if not, create new tree
		rb_tree_create();
	}
	void* ret;
	//If size is zero, print to stderr, continue to allocate
	if(size == 0){
		fprintf(stderr, "Size to be allocated is 0\n");
	}
	
	//allocate
	ret = malloc(size);
	block *newBlock = block_init(ret, size);
	insert_blk(newBlock);
	//return pointer
	return ret;
}

/*
 * Free the memory pointed to by the given pointer.
 * Must check that memory is currently allocated, has not been freed, 
 * the pointer points to first byte of the allocated block
 * If error detected, exit(-1).
 * 
 * Params:
 * ptr: Pointer to memory to be freed
 */
void free537(void *ptr){
	//check if tree exists
	if(!tree_check()){
		fprintf(stderr, "free attempted when no blocks have been allocated yet\n");
		exit(-1);
	}

	//find desired block in tree
	block *to_check = block_init(ptr, 0);
	block *actual = rb_tree_find(to_check); 
	if(!actual) {
		fprintf(stderr, "ptr points to memory not alloc'd by 537malloc or not a start addr\n");
		exit(-1);
	}

	//check that it has not been freed
	if(actual->freed == true) {
		fprintf(stderr, "ptr points to already freed mem in tree\n");
		exit(-1);
	}
	
	//free
	free(ptr);
	free(to_check);
	//indicate freed in tree
	actual->freed = true;
}

/*
 * Free memory pointed to, realloc with specified size.
 * Must check if ptr is NULL, if size is 0.
 * Change tuple in red-black tree
 * 
 * Params:
 * ptr: Points to memory to realloc.
 * size: Desired size of memory to allocate
 * Returns: pointer to new memory
 */
void *realloc537(void *ptr, size_t size){
	//check if tree exists
	if(!tree_check()){
		fprintf(stderr, "realloc attempted when no blocks have been allocated yet\n");
		exit(-1);
	}

	if(!ptr) {
		return malloc537(size);
	}
	
	if(size == 0) {
		fprintf(stderr, "Warning: reallocating with size 0\n");
		free537(ptr);
		//return value doesn't give any new information, mem is freed	
		return NULL;
	}
	
	block *to_check = block_init(ptr, size);
	block *actual = rb_tree_find(to_check);
	//find desired block in tree
	if(!actual) {
		fprintf(stderr, "couldn't find block that needs realloc\n");
		exit(-1);
	}
	//check that it has not been freed
	remove_blk(actual);
	block *new_block = block_init(realloc(ptr, size), size);
	insert_blk(new_block);
	return(new_block->ptr);
}

/*
 * Checks if specified block of memory has been allocated by 537malloc.
 * If error, exit(-1).
 * 
 * Params:
 * ptr: Points to beginning of specified piece of memory
 * size: Number of bits in specified block.
 */
void memcheck537(void *ptr, size_t size){
	//check if tree exists
	if(!tree_check()){
		fprintf(stderr, "memcheck attempted when no blocks have been allocated yet\n");
		exit(-1);
	}

	int cmp = 0;				//holds comparator value
	void  *end = ptr + size;	//points to end of specified block
	void  *tmp_b;
	void  *tmp_e;
	struct block *tmpblk;

	//get root node, store in tmp
	struct rb_node *tmp = get_root();
	
	//find desired block in tree
	while(tmp){
		tmpblk = tmp->memblock;		//get tmp's memblock
		tmp_b = tmpblk->ptr;
		cmp = tmp_b > ptr;	//compare pointers
		if(cmp){ //tmp is greater than specified, get left
			tmp = tmp->link[0];
		}
		else{	 //tmp is l.t. or equal, check if block is w/in this block
			tmp_e = tmp_b + tmpblk->size;
			cmp = tmp_e >= end;
			if(cmp){	//tmp end is greater, it's a hit
				//check if freed
				if(tmpblk->freed){
					fprintf(stderr, "block checked by memcheck has been freed already\n");
					exit(-1);
				}
				return; //if the block is mallocated
			}
			else{		//tmp is less, get right
				tmp = tmp->link[1];
			}
		}
	}
	fprintf(stderr, "block checked by memcheck has not been allocated\n");
	exit(-1);
}

/*
 * Initializes a new block
 *
 * Params:
 * *newptr: addr to start of block
 * newsize: size of block
 */
block * block_init(void *newptr, size_t newsize){
	block *new = malloc(sizeof(block));
	new->ptr = newptr;
	new->size = newsize;
	new->freed = false;
	return new;
}

/* 
 * Inserts block into rb_tree. If block pointed to by malloc is not
 * in tree, checks to see if addr is in a free block's addr range
 * amd if blpck is in deallocated apce but overlaps a free blocks addr range.
 * Handles each situation appropriately by creating and resizing blocks.
 * 
 * Params: 
 * to_add: block to add to tree
 */
static void insert_blk(block *to_add) {
	if(tree_size() == 0) {
		insert_helper(to_add);
		return;
	}

	int change = 1;						//records if change was made to tree
	void  *ptr = to_add->ptr;
	void  *end = ptr + to_add->size;	//points to end of specified block
	void  *tmp_b;
	void  *tmp_e;
	struct block *tmpblk;

	//get root node, store in tmp
	struct rb_node *tmp;
	
	//fix blocks in tree to make room for to_add
	while(change){
		change = 0;
		tmp = get_root();
		while(tmp){
			tmpblk = tmp->memblock;		//get tmp's memblock
			tmp_b = tmpblk->ptr;		//get beginning of tmp block
			tmp_e = tmp_b + tmpblk->size;	//get end of tmp block
			bool free = tmpblk->freed;

			if(tmp_b < ptr){ 			//tmp is less than specified blk, check if w/in
				
				//***CASE 3***
				if(free && end < tmp_e){		//to_add is w/in tmp
					overlap_handler(tmpblk, to_add, CASE3);
					change = 1;
					break;
				}

				//***CASE 2***
				else if(free && (tmp_e == end || (ptr <= tmp_e && tmp_e < end))){
					overlap_handler(tmpblk, to_add, CASE2);
					change = 1;
					break;
				}
				
				tmp = tmp->link[1];
			}
			else{	 //tmp is g.t. or equal, check if block is w/in
					 //ptr <= tmp_b

				//***CASE 1***
				if(free && end < tmp_e && tmp_b <= end){
					overlap_handler(tmpblk, to_add, CASE1);
					change = 1;
					break;
				}

				//***CASE 4***
				else if(free && (tmp_b < end || (ptr == tmp_b && end == tmp_e))){//tmp is w/in to_add
					overlap_handler(tmpblk, to_add, CASE4);							//or they are equal
					change = 1;
					break;
				}
				
				tmp = tmp->link[0];
			}
		}
	}
	insert_helper(to_add);
}

/*
 * Does actual insertion. Helper method.
 *
 * Params:
 * value*: points to block to insert
 */
static void insert_helper(block *value) {
	if(!rb_tree_insert(value)) {
		fprintf(stderr, "rb_tree_insert failed to insert a new node\n");
		exit(-1);
	}
}

/*
 * Remove block heper method
 *
 * Params:
 * value*: points to block to insert
 */
static void remove_blk(block *value) {
	if(!rb_tree_remove(value)) {
		fprintf(stderr, "rb_tree_remove failed to remove a node\n");
		exit(-1);
	}
}


