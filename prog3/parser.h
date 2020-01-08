#ifndef __parser_h__
#define __parser_h__

void parse(char *argv[]);
void printError(int line, int error);
void checkMalloc(char* check, char* error);
char** createList(char* ln, char* array[]);
char* replaceFlags(char* ln, char* flags[], int num);
char* substitute(char *str, char* to_replace, char *sub);
char* createCopy(char* ln);

#endif // __parser_h__
