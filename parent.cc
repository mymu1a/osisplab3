#include <cstdio>
#include <string.h>
#include <unistd.h>
#include <cerrno>
#include "global.h"


//SIGUSR1 = 0;
//SIGUSR2 = 1;

int startChild(char* pathChild, char* pathEnv, unsigned indexChild);

int createChild(unsigned int indexChild, char* pathChildEnv, char* dirChild)
{
	printf("createChild ST\n");
	char	pathChild[256] = { 0, };
	pid_t	pid;
	printf("path=%s\n", dirChild);
	printf("path=%s\n", CHILD_NAME_PROGRAM);

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
	printf("createChild OK\n");

	return pid;
}
#include <signal.h>

void waitSignal()
{
	perror("waitSignal ST\n");
	sigset_t sigToWait;

	sigemptyset(&sigToWait);
	if (sigaddset(&sigToWait, SIGUSR1) != 0)
	{
		printf("errno in sigaddset\n");
		return;
	}
	if (sigaddset(&sigToWait, SIGKILL) != 0)
	{
		printf("errno in sigaddset\n");
		return;
	}
	int signal;

	int err = sigwait(&sigToWait, &signal);
	if (err != 0)
	{
		perror("sigwait error\n");
///		exit(1);
	}

	perror("waitSignal OK\n");
}

int startChild_2(char* path, char* pathEnv, unsigned index)
{
	printf("startChild ST\n");
	waitSignal();		/////
	printf("startChild OK\n");

	return 1;
}

int startChild(char* path, char* pathEnv, unsigned index)
{
	printf("startChild ST\n");

	char* argv[] = { NULL, NULL, NULL };
	char nameProgram[9] = { 0, };

	sprintf(nameProgram, "child_%02d", index);
	argv[0] = nameProgram;
	argv[1] = pathEnv;

	printf("Before exec child process \n");
	printf("path=%s", path);
	execve(path, argv, NULL);

	// we get here only in case of Error

	printf("Error in execve(): %d\n", errno);
	printf("startChild OK\n");

	return 0;
}

