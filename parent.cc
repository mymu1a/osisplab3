#include <cstdio>
#include <string.h>
#include <unistd.h>
#include <cerrno>
#include "global.h"

int startChild(char* pathChild, char* pathEnv, unsigned indexChild);


int createChild(unsigned int indexChild, char* pathChildEnv, char* dirChild)
{
	char	pathChild[256] = { 0, };
	pid_t	pid;

	sprintf(pathChild, "%s%s", dirChild, CHILD_NAME_PROGRAM);
	pid = fork();

	if (pid == -1)
	{
		printf("Error in fork(): %d\n", errno);
		return -1;
	}
	if (pid == 0)
	{
		startChild(pathChild, pathChildEnv, indexChild);
		return -1;
	}
	return pid;
}

int startChild(char* path, char* pathEnv, unsigned index)
{
	char* argv[] = { NULL, NULL, NULL };
	char nameProgram[9] = { 0, };

	sprintf(nameProgram, "child_%02d", index);
	argv[0] = nameProgram;
	argv[1] = pathEnv;

	execve(path, argv, NULL);

	// we get here only in case of Error

	printf("Error in execve(): %d\n", errno);

	return 0;
}
