////////////////////////////////////////////////////////////////////////////////
// Main File:	main.c
// This File:	parser.c
// Other Files:	traverse.c, graph.c, build.c
// 
// Authors:		Deryk Clary 	& 	Chris Sullivan
// Emails:		dclary@wisc.edu & 	csullivan@wisc.edu
// NetIDs:		dclary			&	csullivan9
// CS Logins:	deryk			&	csullivan
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "graph.h"
#include "traverse.h"

void parse(char *argv[]);
void printError(int line, int error);
void checkMalloc(char* check, char* error);
char** createList(char* ln, char* array[]);
char* replaceFlags(char* ln, char* flags[], int num);
char* substitute(char *str, char* to_replace, char *sub);
char* createCopy(char* ln);

static const int BUFFER_SIZE = 1024;
static const int LIST_SIZE   = 100;
typedef int bool;
#define true 1
#define false 0

/*
 * Parses the makefile and creates a linked list with targets and dependencies
 * 
 * argv[]: Arguments passed from the command line
 */
void parse(char *argv[]){
	FILE   *fp;				//points to open makefile
	int     lineNum = 0;	//holds the line number for error cases
	//int     index 	= 0;	//holds the index of the current target in targetList
	//int     j		= 0;	//position in dependents array
	//int     num		= 0;	//number of dependents that a target has
	int		numFlags = 0;	//number of flags found in makefile 
	int 	num_cmds = 0; 
	bool	buffEmpty = true;
	bool	targetLine = false;	
	char   **flags = malloc(BUFFER_SIZE * sizeof(char*));		//points to flags
	char   *targetName;		//name of target parsed from target line
	//char   *counter;		//helps count number of dependents
	char   **dep;			//dependencies of target
	//char  **args;			//command line args for target
	char   *line = malloc(BUFFER_SIZE);	//holds current line
	char   **buff = malloc(BUFFER_SIZE * sizeof(char*)); //buffer for target line with all command lines
	//char **cmds = malloc(BUFFER_SIZE * sizeof(char*));
	char   *curr;			//used for string tokenizing
	char   *copy;
	char* new_line;
	//target *newTarget;		//points to new target struct for building curr target
	target *targetList[LIST_SIZE]; //hold all of the targets that are made
	int numTargets = 0;
	
	argv = NULL;

	//check malloc return
	if(line == NULL){
		printf("Failed to allocate memory in parser\n");
		exit(0);
	}
	
	//open file named either makefile or Makefile, check return
	if((fp = fopen("makefile", "r")) == NULL){
		if((fp = fopen("Makefile", "r")) == NULL){
			printf("Failed to find file named makefile or Makefile\n");
			exit(0);
		}
	}
	//loops through file until NULL is found
	while(fgets(line, BUFFER_SIZE, fp)){
		lineNum++;	//increment line #
		//check if blank line or comment
		if(strcmp(line, "\n") == 0 || strchr(line, '#') != NULL || strcmp(line, "\0") == 0 || line[0] == '\r'){			
			continue;	//skip current line
		}
		//check if line begins with space, error
		if(line[0] == ' '){
			printError(lineNum, 1);
		}

		if(line[0] == '\t' || strchr(line, ':') == NULL) targetLine = false;
		else { 
			targetLine = true;
		}
		//check if it is a flag line
		copy = createCopy(line);
		curr = strtok(copy, "=");
		if(curr != NULL && strcmp(curr, line) != 0){	//if there is a flag here
			flags[numFlags] = malloc(strlen(line) + 1);
			checkMalloc(flags[numFlags], "creating flags");
			strcpy((flags[numFlags]), line);
			numFlags++;
			free(copy);
			continue;
		}
		free(copy);
		copy = NULL;
		
		//check for flags in the line
		copy = createCopy(line);
		char* toFree = copy;
		curr = strchr(copy, '$');
		if(curr != NULL){ //put args from flag into buffer
			char* ret_val = replaceFlags(line, flags, numFlags);
			if((new_line = strchr(line, '\n')) != NULL)
				*new_line = '\0';
			strcpy(line, ret_val);
			free(ret_val);
		}
		free(toFree);
		copy = NULL;
		
		//check if target line
		if(targetLine){	
			if((new_line = strchr(line, '\n')) != NULL || (new_line = strchr(line, '\r')) != NULL)
				*new_line = '\0';
			//get target name
			char* quickcopy;
			char* temp_name;
			quickcopy = createCopy(line);
			toFree = quickcopy;
			temp_name = strtok(quickcopy, ":");
			targetName = malloc(strlen(temp_name) + 1);
			targetName = strcpy(targetName, temp_name);
			//should be just part of line with dependencies
			dep = malloc(sizeof(char*)*BUFFER_SIZE);
			copy = createCopy(line);
			int num_deps = 0;
			char* check;
			char* this_dep = strtok(copy, " ");
			if(this_dep != NULL && strcmp(this_dep, line) != 0) {
				check = createCopy(this_dep);
				this_dep = strtok(NULL, " ");
				while(this_dep != NULL && strcmp(check, this_dep) != 0) {
					free(check);
					dep[num_deps] = malloc(sizeof(char)*(strlen(this_dep) + 1));
					strcpy(dep[num_deps++], this_dep);
					check = createCopy(this_dep);
					this_dep = strtok(NULL, " ");
				}
				free(check);
			}
			free(copy);
			if(numTargets > 0) {
				targetList[numTargets - 1]->args = buff;
				targetList[numTargets - 1]->num_args = num_cmds;
				targetList[numTargets++] = createTarget(targetName, NULL, dep, 0, num_deps);
			}
			else {
				targetList[numTargets++] = createTarget(targetName, NULL, dep, 0, num_deps);
			}
			buffEmpty = true;
			num_cmds = 0;
			free(toFree);
		} //end target line

		//else, the line is a command line
		//add all command lines to buffer with the target
		else{
			if(buffEmpty) {
				free(buff);
				buff = malloc(BUFFER_SIZE * sizeof(char*));
				if(buff == NULL) {
					printf("couldn't allocate mem for buff\n");
					exit(0);
				}
			}
			if((new_line = strchr(line, '\n')) != NULL || (new_line = strchr(line, '\r')) != NULL)
				*new_line = '\0';
			buff[num_cmds] = malloc(sizeof(char) * (strlen(line) + 1));
			strcpy(buff[num_cmds++], line); //add line to buffe
			buffEmpty = false;
		}
	} //end while loop
	free(buff);
	free(flags);
	/*
	 * Check dependencies, make targets for them if they are not made already
	 */
	int match = 0;
	char* to_add;
	int it = numTargets;
	for(int i = 0; i < it; i++) {
		if(targetList[i] == NULL)
			continue;
		for(int j = 0; j < targetList[i]->num_deps; j++) {
			for(int k = 0; k < numTargets; k++) {
				if(k == i) {
					continue;
				}
				else {
					if(strcmp(targetList[i]->dependencies[j], targetList[k]->name) == 0) {
						match = 1;
					} 
				}
			}
			if(match == 0) {
				to_add = malloc(sizeof(targetList[i]->dependencies[j])+1);
				strcpy(to_add, targetList[i]->dependencies[j]);
				char* delete;
				if((delete = strchr(to_add, '\r')) != NULL) {
					*delete = '\0';
				}
				targetList[numTargets++] = createTarget(to_add, NULL, NULL, 0, 0);
			}
			match = 0;
		}
	}
	
	//free allocated memory and close file
	free(line);
	line = NULL;
	pclose(fp);
	populate(argv, targetList, numTargets);	//Call populate to create the graph
	/*for(int i = 0; i < numTargets; i ++) {
		printf("TARGET NAME: %s\n", targetList[i]->name);
		for(int j = 0; j < targetList[i]->num_deps; j++) {
			printf("DEPDENDENCY %d: %s\n", j, targetList[i]->children[j]->name);
		}
		
	}*/
	execute(targetList[0]);			//Do things
}

/*
 * Prints any error cases the program encounters while parsing lines
 * Exits after printing.
 *
 * line: Line number
 * error: Error number, makes it easier to debug and see what's going wrong
 */
void printError(int line, int error){
	printf("%d: Invalid line. Error %d.\n", line, error);
	exit(0);
}

/*
 * Checks that the pointer (check) was allocated without error
 */
void checkMalloc(char* check, char* error){
	if(check == NULL){
		printf("Failed to allocate memory for %s\n", error);
		exit(0);
	}
}

/*
 * Creates lists of strings for the arguments and dependencies
 * within the lines of the make file by parsing by spaces
 * 
 * ln: Pointer to the line that needs a list to be made out of the elements
 */
char** createList(char* ln, char* array[]){
	char* copy;		//takes a copy of the string
	char* toFree;
	int i = 0;		//holds place in array
	int count = 0;	//holds number of element to go into array

	//check if ln is null
	if(ln == NULL) printError(999, 999);

	//get rid of tab, if there is one
	if(*ln == '\t') ln = ln+1;
	if(*ln == '\t') { //if there is a double tab
		printf("Error: double tab found\n");
		exit(0);
	}

	//create copy of the line so that tokenizer doesn't jack it up
	copy = createCopy(ln);
	toFree = copy;

	//count number of spaces needed
	copy = strtok(ln, " ");
	while(copy != NULL){		//count number of pointers will be needed
		count++;
		copy = strtok(NULL, " ");
	}
	free(toFree);
	copy = NULL;

	if(count == 0) return NULL;

	//malloc array for pointers
	array = malloc(sizeof(char*) * (count));
	if(array == NULL){
		printf("Failed to malloc array\n");
		exit(0);
	}

	//create copy of the line so that tokenizer doesn't jack it up
	copy = createCopy(ln);
	toFree = copy;

	//Start tokenizing string by spaces
	copy = strtok(ln, " ");
	while(i < count && copy != NULL){	
		//check for comment
		if(*copy == '#' || *copy == '\n') break;
		
		//malloc memory for the element
		array[i] = malloc(strlen(copy) + 1);
		checkMalloc(array[i], "array element");

		//add word to list
		strcpy(array[i], copy);
		i++;

		//get next
		copy = strtok(NULL, " ");
	}
	free(toFree);
	toFree = NULL;
	return array;
}

/*
 * Replaces any flags that are found in a line with the arguments they represent
 *
 * ln:		Line with possible flags in it
 * flags: 	Array with all the known flags in it
 * num:		Number of flags found
 *
 * Returns: Pointer to edited string
 */
char* replaceFlags(char* ln, char* flags[], int num) {
	//find first flag
	char* pointer;
	char* args_pointer;
    char* flag;	
	char* args_replace = malloc(sizeof(char) * BUFFER_SIZE);
	//char* flag_pointer;
	char* flag_flag;
	char* flag_copy;
	char* copy; 
	char* ret; 
	char* to_replace = malloc(sizeof(char) * BUFFER_SIZE);
	char* toFree = malloc(1); //make sure free at end of function is able to free something
	char* freeFlag;
	
	ret = createCopy(ln);
	pointer = strstr(ret, "$(") + 2;
	while(pointer != NULL) {
		pointer += 2;
		copy = createCopy(pointer);
		free(toFree);
		toFree = copy;

		//flag now stores flag found in ln
		flag = strtok(copy, ")");
		
		//find out which flag and get args for that flag
		for(int i = 0; i < num; i++) {
			flag_copy = createCopy(flags[i]);
			freeFlag = flag_copy;
			flag_flag = strtok(flag_copy, " ");
			//if flags match get args for this flag and replace in og string
			if(strcmp(flag, flag_flag) == 0) {
				//points to beginning of argument list
				args_pointer = strchr(flags[i], '=') + 2;
				//copy rest of arguments into args
				
				char* temp = args_pointer + strlen(args_pointer) - 2; //check if we are omitting a character
				if(temp != NULL && isalpha(*temp)){
					strncpy(args_replace, args_pointer, strlen(args_pointer) - 1);
				} 
				
				
				else {
					strncpy(args_replace, args_pointer, strlen(args_pointer) - 2);
				}
				
				sprintf(to_replace, "%s%s%s", "$(", flag, ")");

				ret = substitute(ret, to_replace, args_replace);
			}
			free(freeFlag);
		}
		pointer = strstr(ret, "$(");
	}
	free(toFree);
	free(args_replace);
	free(to_replace);
	return ret;
}

char* substitute(char *str, char* to_replace, char *sub) {
	char* buffer;
	if((buffer = malloc(sizeof(char) * BUFFER_SIZE)) == NULL) {
		printf("Failed to allocate mem on line: %d\n", __LINE__ - 1);
		exit(0);
	}

	char *p = NULL;
	//set p to point to first instance of string to be replaced in og string
	if(!(p = strstr(str, to_replace))) {
		free(buffer);
		return str;
	}
	
	//copy starting chars from str into buff
	strncpy(buffer, str, p-str);
	//copy the sub string into buffer 
	sprintf(buffer + (p - str), "%s%s", sub, p + strlen(to_replace));

	return buffer;
}



/*
 * Creates a copy of the line provided and returns pointer to the copy
 */
char* createCopy(char* ln){
	char* copy;

	//create copy
	copy = malloc(strlen(ln) + 1);
	checkMalloc(copy, "createCopy");
	strcpy(copy, ln);

	return copy;
}
