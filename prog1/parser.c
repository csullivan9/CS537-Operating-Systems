////////////////////////////////////////////////////////////////////////////////
// Main File:	options.c
// This File:	parser.c
// Other Files:	processes.c
// 
// Authors:		Deryk Clary 	& 	Chris Sullivan
// Emails:		dclary@wisc.edu & 	csullivan9@wisc.edu
// CS Logins:	deryk			&	csullivan
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int BUFF = 1000;
//lines to parse to for desired info in file
const int V_MEM = 17, STATE_INFO = 2, 
	USER_TIME = 13, SYSTEM_TIME = 14;
FILE *fp; //points to open file

/*
 * This function finds and returns the information from either stat or status
 * file that is requested from the command line flag. Parses and returns.
 *
 * char *command	Command line flag that indicates desired information
 * int pid			PID of process to parse stat or status file for
 */
char* stat_parser(int pid, char *command) { 
	//string pointer
	char *line = malloc (sizeof (char) * BUFF);
	char *to_ret = malloc (sizeof (char) * BUFF);
	//check malloc return value
	if(line == NULL || to_ret == NULL) {
		printf("Failed to allocate memory in parser\n");
		return 0;
	}

	char filename[BUFF]; //create filename string
	int numLines = 0; //line with desired information

	sprintf(filename, "/proc/%d/status", pid);
	if((fp = fopen(filename, "r")) == NULL) perror("fopen");

	if(strcmp(command, "-v") == 0) { //Virtual memory (in pages)
		sprintf(filename, "/proc/%d/status", pid);
		if((fp = fopen(filename, "r")) == NULL) perror("fopen");

		//parse virtual mem info from statm
		numLines = V_MEM;
		//scan through 18 lines
		while(numLines >= 0) {
			numLines--; 
			if(fgets(line, BUFF, fp) == NULL) {
				printf("fgets() failed to parse v_mem line\n");
				return 0;
			} 
		}
		fclose(fp); //Close file
		int size; //used to store v_mem value parsed
		to_ret = strtok(line, "\n");
		if(to_ret == NULL) {
			printf("strtok() in parser has returned a null value\n");
			return 0;
		}

		return to_ret;
	}
		
	else if(strcmp(command, "-s") == 0) { //State information
		sprintf(filename, "/proc/%d/status", pid);
		if((fp = fopen(filename, "r")) == NULL) perror("fopen");
		numLines = STATE_INFO;
		//scan through 3 lines
		while(numLines >= 0) {
			numLines--;
			if(fgets(line, BUFF, fp) == NULL) {
				printf("fgets() failed to parse state info line\n");
				return 0;
			}
		}

		fclose(fp); //Close file
		if(strsep(&line, "\t") == NULL) {
			printf("strsep() in parser has failed");
			return 0;
		}
		sprintf(to_ret, "%s", strsep(&line, " "));
		return to_ret;
	}
		
	else if(strcmp(command,"-U") == 0) { //User time
		sprintf(filename, "/proc/%d/stat", pid);
		if((fp = fopen(filename, "r")) == NULL) perror("fopen");
		numLines = USER_TIME;
		if(fgets(line, BUFF, fp) == NULL) {
			printf("failed to parse user time line");
			return 0;
		}
		while(numLines >= 1) {
			numLines--;
			//printf("%s", line);
			if(strsep(&line, " ") == NULL) {
				printf("strsep() in parser has failed\n");
				return 0;
			}
		}	
		to_ret = strtok(line, " ");
		if(to_ret == NULL) {
			printf("strtok() has returned a null value\n");
			return 0;
		}

		fclose(fp); //Close file
		return to_ret;
	}
		
	else if(strcmp(command, "-S") == 0) { //System time
		sprintf(filename, "/proc/%d/stat", pid);
		if((fp = fopen(filename, "r")) == NULL) perror("fopen");
		numLines = SYSTEM_TIME;
		if(fgets(line, BUFF, fp) == NULL) {
			printf("failed to parse system time line\n");
			return 0;
		}
		while(numLines >= 1) {
			numLines--;
			if(strsep(&line, " ") == NULL) {
				printf("strsep() in parser has failed\n");
				return 0;
			}
		}
		to_ret = strtok(line, " ");
		if(to_ret == NULL) {
			printf("strtok() has returned a null value\n");
			return 0;
		}
		if(to_ret == NULL) {
			printf("strsep() in parser has failed\n");
			return 0;
		}
		fclose(fp); //Close file
		return to_ret;	
	}

	return NULL;
}

