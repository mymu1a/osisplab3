#include "global.h"

#include <cerrno>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

//							signum.h
// #define SIGUSR1		10	/* User-defined signal 1.  */
// #define SIGUSR2		12	/* User-defined signal 2.  */

const char* nameProgram = NULL;
bool printStatistic = true;

//=== statistic ===
int count_00 = 0;
int count_01 = 0;
int count_10 = 0;
int count_11 = 0;

short val[2] = { 0, 0 };


void signalHandler(int numberSignal)
{
	fprintf(stderr, "signalHandler: %d\n", numberSignal);
	switch (numberSignal)
	{
	case SIGUSR1:		printStatistic = false; break;
	case SIGUSR2:		printStatistic = true; break;
	default:
		fprintf(stderr, "Unexpected Signal number: %d\n", numberSignal);
	}
}

void* thread_function(void* arg)
{
	short index = 0;
	short value = 0;

	while (1)
	{
		val[index] = value;
		if (++index > 1)
		{
			index = 0;
			value ^= 1;
		}
///		printf("\t\t<< second thread --- %d-%d\n", val[0], val[1]);
		usleep(145000);
	}
}

void readStatistic()
{
	if (val[0] == 0 && val[1] == 0) count_00++;
	if (val[0] == 0 && val[1] == 1) count_01++;
	if (val[0] == 1 && val[1] == 0) count_10++;
	if (val[0] == 1 && val[1] == 1) count_11++;
}

void withNanoSleep()
{
	struct timespec		time, time2;
	int					count = 25;
	int					result;
	short				tickInCircle = 4;
	short				tick = tickInCircle;

	time.tv_sec = 0;
	time.tv_nsec = 0.5 * 1000000000;

	count *= tickInCircle;
	while (count-- > 0)
	{
		result = nanosleep(&time, &time2);
		if (result == 0)
		{
			readStatistic();
			if (tick == 0)
			{
				if (printStatistic == true)
				{
					printf("\t%s\t\t%d:%d:%d:%d\t( pid: %d, ppid: %d)\n", nameProgram, count_00, count_01, count_10, count_11, getpid(), getppid());
				}
				tick = tickInCircle;
			}
			tick--;
		}
		if (result == -1)
		{
			if (errno != 4)
			{
				printf("Error in nanosleep():\n");
				printf("  errno: %d\n", errno);
				printf("  errno: %s\n", strerror(errno));

				return;
			}
		}
	}
}


int main(int argc, char* argv[])
{
	printf("child process ST\n");

	int					result;
	struct sigaction	action;

	action.sa_handler = signalHandler;
	action.sa_flags = 0;

	//=== connect to Signals ===

	result = sigaction(SIGUSR1, &action, NULL);						// SIGUSR1
	if (result == -1)
	{
		printf("cannot connect to Signal: SIGUSR1\n");

		printf("child process OK\n");
		return 1;
	}

	result = sigaction(SIGUSR2, &action, NULL);						// SIGUSR2
	if (result == -1)
	{
		printf("cannot connect to Signal: SIGUSR2\n");

		printf("child process OK\n");
		return 1;
	}
	// generate 0-1 pairs in separate thread
	pthread_t	idThread;

	pthread_create(&idThread, NULL, &thread_function, NULL);
	
	//=== main loop ===
	nameProgram = argv[0];

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
