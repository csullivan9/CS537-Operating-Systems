////////////////////////////////////////////////////////////////////////////////
// Main File:	options.c
// This File:	processes.c
// Other Files:	parser.c
// 
// Authors:		Deryk Clary 	& 	Chris Sullivan
// Emails:		dclary@wisc.edu & 	csullivan@wisc.edu
// CS Logins:	deryk			&	csullivan
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "parser.h"
#include "options.h"

#define BUFF_SIZE 1024

/*
 * This function finds the list of processes belonging to the user
 *
 * param *argv[]	List of arguments from the command line
 * param size		Number of elements in argv
 */
void processes(char *argv[], int size) {
	DIR *procDir;					//Points to the open directory
	struct dirent *entry;			//Points to curr directory entry
	int uid = getuid();				//ID of the user
	FILE *fp;						//Holds open file
	char buff[BUFF_SIZE];			//Buffer
	
	procDir = opendir("/proc/");	//Open /proc
	if(procDir == NULL)				//Check if opendir() was successful
		{printf("error: could not open directory"); exit(0);}
	else{							//Else, return dir
		while((entry = readdir(procDir)) != NULL){
			int pid = atoi(entry->d_name);
			if(entry->d_type & DT_DIR && pid != 0){		//Only look through directories, not files
														//Check if curr entry is a number
														//All processes have numbered directories
				//create filename
				char filename[256];
				sprintf(filename, "/proc/%d/status", pid);
				
				//parse uid
				if((fp = fopen(filename, "r")) == NULL){ //Open and check completion
					printf("error: could not open file"); exit(0);
				}
				fgets(buff, BUFF_SIZE, fp); //Get first line from file
				int lines = 8;
				while(lines--) fgets(buff, BUFF_SIZE, fp); //Scan through 8 lines
				char junk[100];
				int currUid;

				//Gets first number from buff, which is UID of curr process
				sscanf(buff, "%s\t%d", junk, &currUid);
				
				//if uid matches, call options with args for this process
				if(uid == currUid) check_flags(pid, argv, size);
				
				//else move to next directory (do nothing)
					
				fclose(fp); //Close the file
			}
		}
		closedir(procDir); //Close the directory
	}
}
