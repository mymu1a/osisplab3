#include "global.h"

#include <cerrno>
#include <signal.h>
#include <stdio.h>
#include <time.h>

/*
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
//*/
void signalHandler(int signum)
{
	printf("signalHandler ST\n");

	printf("signum: %d\n", signum);

	printf("signalHandler OK\n");
}

void withNanoSleep()
{
	struct timespec		time;
	int					count = 15;
	int					result;

	time.tv_sec = 2;

	while (count-- > 0)
	{
		result = nanosleep(&time, NULL);
		if (result == 0)
		{
			printf("Time elapsed\n");
		}
		if (result == -1)
		{
			printf("signal\n");
			printf("errno: %d\n", errno);
		}
	}
}

void waitSignal()
{
	printf("waitSignal ST\n");
	sigset_t sigToWait;

	if (sigaddset(&sigToWait, SIGUSR1) != 0)
	{
		printf("errno in sigaddset\n");
		return;
	}
	int signal;

	sigwait(&sigToWait, &signal);
	printf("waitSignal OK\n");
}

int main(int argc, char* argv[])
{
	printf("child process ST\n");
	int					result;
	struct sigaction	action;

	action.sa_handler = signalHandler;
	action.sa_flags = 0;
	
	// connect to Signals
	//*
	result = sigaction(SIGUSR1, &action, NULL);
	if (result == -1)
	{
		printf("cannot connect to Signal: SIGUSR1\n");

		printf("child process OK\n");
		return 1;
	}
	result = sigaction(SIGUSR2, &action, NULL);
	if (result == -1)
	{
		printf("cannot connect to Signal: SIGUSR2\n");

		printf("child process OK\n");
		return 1;
	}
	//*/

///	waitSignal();
	withNanoSleep();


	/*
	if (argc < 2)
	{
		printf("Usage: %s <file-child-env>\n", argv[0]);
		printf("child process OK\n");
		return 1;
	}
	printf("Child program name: %s\n", argv[0]);
	printf("Child program pid: %d\n", getpid());
	printf("Child program parent pid: %d\n", getppid());
	printf("Path to ChildEnv: %s\n", argv[1]);
	printf("\n");
	//*/
	printf("child process OK\n\n");

	return 0;
}