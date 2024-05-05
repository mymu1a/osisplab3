#include "global.h"

#include <cerrno>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/*
#include <unistd.h>
#include <stdlib.h>
//*/
void signalHandler(int signum)
{
	perror("signalHandler ST\n");

	printf("signum: %d\n", signum);

	perror("signalHandler OK\n");
}

void withNanoSleep()
{
	struct timespec		time, tim2;
	int					count = 15;
	int					result;

	time.tv_sec = 2;

	while (count-- > 0)
	{
		result = nanosleep(&time, &tim2);
		if (result == 0)
		{
			printf("Time elapsed\n");
		}
		if (result == -1)
		{
			printf("signal\n");
			printf("errno: %d\n", errno);
			printf("errno: %s\n", strerror(errno));
			
		}
	}
}

void waitSignal()
{
	printf("waitSignal ST\n");
	sigset_t sigToWait;

	sigemptyset(&sigToWait);
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

	/*
	sigset_t oldmask_1, oldmask_2, newmask_1, newmask_2;


	sigemptyset(&newmask_1);
	sigaddset(&newmask_1, SIGUSR1);
	sigprocmask(SIG_BLOCK, &newmask_1, &oldmask_1);

	sigemptyset(&newmask_2);
	sigaddset(&newmask_2, SIGUSR2);
	sigprocmask(SIG_BLOCK, &newmask_2, &oldmask_2);
	//*/

	action.sa_handler = signalHandler;
	action.sa_flags = 0;
	action.sa_flags = SA_SIGINFO;

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