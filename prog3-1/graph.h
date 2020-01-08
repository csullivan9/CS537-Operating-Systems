#ifndef __graph_h__
#define __graph_h__
#define SIZE 1024

typedef struct Target {
	//string representation of the name of this target
	char *name;
	//list of arguments for creation of target
	char **args;
	//string representation of dependencies
	char **dependencies;
	int num_args;
	int num_deps;
	//this will hold the pointers to this target's dependencies
	int parents;
	struct Target *children[100];
} target;

void populate(char* argv[], target **nodes, int size);
int find(char *name, target **nodes, int size);
void* createTarget(char *name, char **args, char **dependencies, int num_args, int num_deps);
_Bool checkCycle(target **nodes, int size);

#endif // __graph_h__
