////////////////////////////////////////////////////////////////////////////////
// Main File:	main.c
// This File:	graph.c
// Other Files:	traverse.c, build.c, parser.c
// 
// Authors:		Deryk Clary 	& 	Chris Sullivan
// Emails:		dclary@wisc.edu & 	csullivan@wisc.edu
// NetIDs:		dclary			&	csullivan9
// CS Logins:	deryk			&	csullivan
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "graph.h"

typedef struct Target target;
void populate(char* argv[], target **nodes, int size);
int find(char *name, target **nodes, int size);
void* createTarget(char *name, char **args, char **dependencies, int num_args, int num_deps);
bool checkCycle(target **nodes, int size);

/* Populates graph by iterating through target array
 * and setting each targets dependency pointers (children)
 *
 * params: nodes -> array of target structs
 * 		   size  -> size of array 
 */
void populate(char* argv[], target **nodes, int size) {
	(void)argv; //get rid of unused variable warning
	int index = 0;
	//iterate through each target in array
	for(int i = 0; i < size; i++) {
		//get size of this targets dependency list
		int dependencies = nodes[i]->num_deps;
		//iterate through dependency list and set pointers to children
		for(int j = 0; j < dependencies; j++) {
			index = find(nodes[i]->dependencies[j], nodes, size);
			nodes[i]->children[j] = nodes[index];
			//increase childs parent count for cycle detection
			nodes[i]->children[j]->parents++;
		}
	}

	/*if(checkCycle(nodes, size)) {
		printf("Cycle found, exiting");
		exit(0);
	}*/
}

/* Searches through nodes array to find node with same name as that passed in
 * 
 * Params: char *name     -> name of node we are looking for
 * 		   target[] nodes -> array of nodes to search through
 *  	   int size       -> size of array
 * Returns: pointer to position in nodes array that has matching name field
 */
int find(char *name, target **nodes, int size) {
	int index = 0;
	//look for target in array that matches this targets dependency
	//set that target as one of this targets children
    char* eat;
	if((eat = strchr(name, '\r')) != NULL) {
		*eat = '\0';
	}
	while(index < size && strcmp(nodes[index]->name, name) != 0) {
		//printf("%d\n", index++);
		index++;
	}
	return index;
}

void* createTarget(char *name, char **args, char **dependencies, int num_args, int num_deps) {
	target *new = malloc(sizeof(target));
	new->name = name;
	new->args = args;
	new->dependencies = dependencies;
	new->num_deps = num_deps;
	new->parents = 0;
	new->num_args = num_args;
	return new;
}

bool checkCycle(target **nodes, int size) {
	//indices of sorted nodes
	int L[size];
	//indices of nodes with no incoming edges
	bool S[size];
	//indicates if S is empty
	bool s_empty = 0;
	//indicates if graph has a cycle
	bool cycle = 0;
	//index of tail of L
	int index = 0;
	//number of children in current node
	int numChildren = 0;

	//initiate arrays
	for(int i = 0; i < size; i++) {
		L[i] = 0;
		S[i] = 0;
	}
	
	//S starts with just nodes with no parents
	for(int i = 0; i < size; i++) {
		if(nodes[i]->parents == 0) S[i] = 1;
	}

	//while S is not empty
	while(!s_empty) {
		//remove a node from S add to tail of L
		for(int i = 0; i < size; i++) {
			if(S[i]) {
				S[i] = 0;
				L[index] = i;
				index++;
			}
			//for each node in L delete edges
			for(int j = 0; j < size; j++) {	
				//find node
				if(L[j] != 0) {
					numChildren = sizeof(nodes[j]->dependencies) / sizeof(char*);
					//for each 
					for(int k = 0; k < numChildren; k++) {
						nodes[j]->children[k]->parents--;
						if(nodes[j]->children[k]->parents == 0) {
							//find index of dependency in nodes array, add to S
							for(int l = 0; l < size; l++) {
								if(nodes[j]->children[k]->name == nodes[l]->name) {
									S[l] = 1;
									break;
								}
							}
						}
					}
				}
			}
		}

		for(int i = 0; i < size; i++) {
			if(S[i]) {
				s_empty = 0;
				break;
			} else {
				s_empty = 1;
			}
		}
	}

	for(int i = 0; i < size; i++) {
		if(nodes[i]->parents != 0) {
			cycle = 1;
			break;
		}
	}

	return cycle;
}
