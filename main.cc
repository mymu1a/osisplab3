#include "global.h"

#include <cstdio>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



pid_t	pidChild_test = 0;

int createChild(unsigned indexChild, char* pathChildEnv, char* dirChild);
void printUsage(char* nameProgram, short index);
void stopStatistic();


int main(int argc, char* argv[], char* envp[])
{
	printf("main ST\n");
	if (argc < 2)
	{
		printUsage(argv[0], 2);
		printf("main OK\n");
		return 1;
	}
	char* dirChild;

	dirChild = getenv(CHILD_DIR);
	if (dirChild == NULL)
	{
		printUsage(argv[0], 1);
		printf("main OK\n");
		return 1;
	}
	printf("Parent program name: %s\n", argv[0]);
	printf("Parent program pid: %d\n", getpid());
	printf("Path to ChildEnv: %s\n", argv[1]);
	printf("\n");

	printf("Type: + - l k s g s<num> g<num> p<num>\n");

	char ch;
	unsigned indexChild = 0;

	while (ch = getchar())
	{
		switch (ch)
		{
		case '+':
		{
			pid_t	pidChild;

			pidChild = createChild(indexChild++, argv[1], dirChild);
			if (pidChild == -1)
			{ // child or error
				return 0;
			}
			// parent process

			pidChild_test = pidChild;

			break;
		}
		case 's':
		{
			if (pidChild_test != 0)
			{
				stopStatistic();
			}
			break;
		}

			//case '-':

			//	break;
			//case 'l':

			//	break;
			//case 'k':

			//	break;
			//case 'g':

			//	break;
			//case 's':

			//	break;
			//case 'g':

			//	break;
			//case 'p':

			//	break;
			// parent process
		case 'q':
			printf("main OK\n");
			return 0;
		}
	}
	printf("main OK\n");

	return 0;
}

void printUsage(char* nameProgram, short index)
{
	printf("Usage: \n"
		"1. Set Environment variable: %s", CHILD_DIR);

	if (index == 1)
	{
		printf("\t\t<--");
	}
	printf("\n");
	printf("2. %s <file-child-env>", nameProgram);
	if (index == 2)
	{
		printf("\t\t<--");
	}
	printf("\n");
}

void stopStatistic()
{
	kill(pidChild_test, SIGUSR1);
}