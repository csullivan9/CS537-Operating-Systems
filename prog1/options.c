////////////////////////////////////////////////////////////////////////////////
// Main File:	options.c
// This File:	options.c
// Other Files:	parser.c, processes.c
// 
// Authors:		Deryk Clary 	& 	Chris Sullivan
// Emails:		dclary@wisc.edu & 	csullivan@wisc.edu
// CS Logins:	deryk			&	csullivan
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "processes.h"
#include <string.h>

void check_flags(int pid, char *flags[], int size);

const int BUFFER = 1024;

int main(int argc, char *argv[]) {
	int pid;
	//check for valid input
	char *valid[] = {"-p", "-s", "-s-", "-U", "-U-", 
		"-S", "-S-", "-v", "-v-", "-c", "-c-"};
	int check = 0; //boolean for valid input
	int num = 0; //Holds number of ints in args array

	//iterate through args to ensure validity
	for(int i = 1; i < argc; i++){
	check = 0; //reset check
		for(int j = 0; j < 11; j++){ //compare to valid input array
			//check if the input is in the valid array or is a number
			if(strcmp(argv[i], valid[j]) == 0){ //if valid input
				check = 1; break;
			}
			else if(atoi(argv[i]) != 0){ //if number
				//if there is a number in the input, it must be preceded by "-p"
				if(argv[i-1] == NULL || strcmp(argv[i-1], "-p") != 0){
					printf("error: invalid input\n"); return 0;
				}
				check = 1; num++; break;
			}
		}
		//if check = 0, invalid entry found
		if(check == 0){
			printf("error: invalid input\n");
			return 0;
		}
	}
	if(num > 1){ //print error for multiple ints in args
			printf("error: more than one pid requested\n");
			return 0;
	}

	//check if p flag is present
	for(int i = 1; i < argc; i++) {
		if(strcmp(argv[i], "-p") == 0) {
			if(argv[i+1] == NULL){ //check if there is a PID number
				printf("error: input a PID value\n"); 
				return 0;
			}
			pid = atoi(argv[i + 1]); //convert PID into int
			if(pid == 0) {
				printf("error: input a PID value\n"); 
				return 0;
			}
			check_flags(pid, argv, argc);
			return 0;
		} 
	}

	processes(argv, argc); //no p flag, call processes and print all

	return 0;
}

/*
 * Check flags and call parser, passing the command to parser 
 *
 * param pid		PID of the process we want information on
 * param *flags[]	Arguments from the command line
 * param size		Number of arguments in flags array
 */
void check_flags(int pid, char *flags[], int size) {
	char buff[BUFFER];
	int u = 1; //Boolean for utime
	int c = 1; //Boolean for cmdline

	sprintf(buff, "%d: ", pid); //Put PID at beginning of line to print

	for(int i = 1; i < size; i++){ //Iterate through flags
		if(strcmp(flags[i], "-s") == 0 || strcmp(flags[i], "-s-") == 0) {
			//Display state information
			strcat(buff, stat_parser(pid, "-s")); 
			strcat(buff, "\t");
		}

		//Always on, unless "-U-" is present
		else if(strcmp(flags[i], "-U-") == 0) {
			//Don't display amount of user time consumed by process
			u = 0;
		}

		else if(strcmp(flags[i], "-S") == 0 || strcmp(flags[i], "-S-") == 0) { 
			//Display amount of system time consumed by process
			strcat(buff, "systime=");
			strcat(buff, stat_parser(pid, "-S")); 
			strcat(buff, "\t");
		}

		else if(strcmp(flags[i], "-v") == 0 || strcmp(flags[i], "-v-") == 0) {
			//Display amount of virtual memory curr being used (in pages)
			strcat(buff, stat_parser(pid, "-v"));
			strcat(buff, "\t");
		}

		//Always on, unless "-c-" is present
		else if(strcmp(flags[i], "-c-") == 0) { 
			//Don't display commandline that started the process
			c = 0;
		}
	}

	//Add U
	if(u){
		strcat(buff, "utime=");
		strcat(buff, stat_parser(pid, "-U")); 
		strcat(buff, " \t");
	}

	//Add c
	if(c){
		//Create cmdline filepath
		char filename[BUFFER];
		char cmdln[BUFFER];
		char add[BUFFER];
		sprintf(filename, "/proc/%d/cmdline", pid);
	
		//Find cmdline, add to buffer
		FILE *fp;
		if((fp = fopen(filename, "r")) == NULL) //open file, check completion
			{printf("error: could not open cmdline file"); exit(0);}
		if(fgets(cmdln, BUFFER, fp) == NULL){ //copy line to cmdln, check completion
			printf("fgets() failed to parse cmdline file");
			exit(0);
		}
		sprintf(add, "[%s]", cmdln); //Format string before adding to buffer
		strcat(buff, add);
		fclose(fp); //Close file
	}

	printf("%s\n", buff); //Print the buffer to the command line
}



