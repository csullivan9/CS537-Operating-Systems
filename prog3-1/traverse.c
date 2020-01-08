////////////////////////////////////////////////////////////////////////////////
// Main File:	main.c
// This File:	traverse.c
// Other Files:	graph.c, build.c, parser.c
// 
// Authors:		Deryk Clary 	& 	Chris Sullivan
// Emails:		dclary@wisc.edu & 	csullivan@wisc.edu
// NetIDs:		dclary			&	csullivan9
// CS Logins:	deryk			&	csullivan
////////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <ctype.h>
#include "graph.h"

static const int buff = 1024;

void execute(target *curr);
char *getPath(char *filename);
bool compMod(char *t_name, char *d_name);

/* Execute will traverse the graph of dependencies
 * determining what commands need to be executed
 * and executing them.
 * 
 * params: int curr -> current vertex
 */
void execute(target *curr) { 
	//if this is a target vertex check its dependencies
	if(curr->dependencies != NULL) {
		bool rebuild = 0;
		int num_dep = curr->num_deps;
		
		//iterate through dependencies of curr
		for(int i = 0; i < num_dep; i++) {
			//check if this dependency is a target
			if(curr->children[i]->dependencies != NULL) {
				//if target dependency does not exist yet, try to build
				if(fopen(curr->children[i]->name, "r") == NULL) {
					execute(curr->children[i]);
				}
				else { //target does exist check mod time
					rebuild = compMod(curr->name, curr->children[i]->name);	
					
				}
			} 
			else { //else this dependency is just a file
				//see if file exists

				const char* convert = curr->children[i]->name;
				char* pointer;
				if((pointer = strchr(convert, '\n')) != NULL) {
					*pointer = '\0';
				}

				struct stat buffer;
				int exist = stat(convert, &buffer);
			    if(exist != 0) {
					exit(0);
				} else { //file does exist, check mod time
					rebuild = compMod(curr->name, curr->children[i]->name);
				  }
			}
		}
		
		//all dependencies for this target have been checked, see if needs rebuilding
		if(rebuild) {
			pid_t p;
			p = fork();
			if(p < 0) {
				printf("Could not fork\n");
				exit(0);
			}
			
			if(p == 0) {
				
				char *args[buff];
				//char to_add[buff];
				char copy[buff];
				//char prog[buff];
				char *currArgs = curr->args[0];
				currArgs++; //get rid of tab
				strcpy(copy, currArgs);
				int index = 0;
				
				//get cmd
				char* curr = strtok(copy, " ");
				char* cmd = malloc(sizeof(curr) + 1);
				if(cmd == NULL) {
					printf("traverse malloc failed\n");
					exit(0);
				}
				strcpy(cmd, curr);
				
				//print cmd
				curr = strtok(NULL, " "); //get first arg
				
				while(curr != NULL){	//put args into args array
					args[index] = malloc(sizeof(curr) + 1);
					if(args[index] == NULL) exit(0);
					
					strcpy(args[index++], curr);
					curr = strtok(NULL, " ");
				}
				
				execvp(cmd, args);
				free(cmd);
			} 
			else {
				wait(NULL);
			}
		}
	}
}

/* Returns path of file passed in as param by using getcwd()
 *
 * params: char *filename -> string representing name of file
 * returns: filepath of file
 */
char *getPath(char *filename) {
	char *path = getcwd(NULL, buff);
	
	if(path == NULL) {
		printf("Failed to allocate mem for path");
		exit(0);
	}

	if(path != NULL) {
		strcat(path, "/");
		strcat(path, filename);
	} 
	else {
		return NULL;
	}
	return path;
}

/* Compares modification times of target and dependency 
 * 
 * params: char *t_name -> name of target file
 *         char *d_name -> name of dependency file
 * returns: true if target is younger than dependency, false otherwise 
 */
bool compMod(char *t_name, char *d_name) {
	struct stat t_attr;
	struct stat d_attr;
	char *t_path = getPath(t_name);
	char *d_path = getPath(d_name);
	if(stat(t_path, &t_attr) < stat(d_path, &d_attr)) {
		return 1;
	} 
	return 0;
}





