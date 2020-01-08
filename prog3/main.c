////////////////////////////////////////////////////////////////////////////////
// Main File:	main.c
// This File:	main.c
// Other Files:	parser.c, graph.c
// 
// Authors:		Deryk Clary 	& 	Chris Sullivan
// Emails:		dclary@wisc.edu & 	csullivan@wisc.edu
// NetIDs:		dclary			&	csullivan9
// CS Logins:	deryk			&	csullivan
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

int main(int argc, char *argv[]){
	parse(argv);
	(void)argc; //get rid of unused parameter warning
	//Do some stuff
	return 0;
}
