#include "global.h"

#include <cstdio>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/queue.h>
#include <sys/wait.h>
#include <ctype.h>

//===== `Child` Struct =====
struct Child
{
	unsigned	index;
	pid_t		pid;

	TAILQ_ENTRY(Child) allChildren;         /* Tail queue. */
};

//===== Functions =====
int createChild(unsigned indexChild, char* pathChildEnv, char* dirChild);
struct Child* getChild(int indexChild);
void printStatistic(struct Child* pChild);
void printUsage(char* nameProgram, short index);
void stopChild(pid_t pidChild);						// send `SIGKILL` signal to Child
void stopChildAll();								// terminate all Child processes
void startStatistic();
void startStatistic(pid_t pidChild);
void stopStatistic();
void stopStatistic(pid_t pidChild);

//=== Global Variables ===
TAILQ_HEAD(HeadOfCollection, Child) head = TAILQ_HEAD_INITIALIZER(head);
unsigned countChild = 0;


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

	//=== read user input ===
	int ch;
	unsigned indexChild = 0;
	TAILQ_INIT(&head);                      /* Initialize the queue. */

	while ((ch = getchar()) != EOF)
	{
		int charNext;
		struct Child* pChild;

		if (ch == 's' || ch == 'g' || ch == 'p')
		{
			charNext = getchar();
			if (isdigit(charNext))
			{
				pChild = getChild(charNext);
				printf("<< Error:  there is no Child with index: %c\n", charNext);

				continue;
			}
			else
			{ // return 'char' back to the stream
				ungetc(charNext, stdin);
			}
		}
		//=== select command ===
		switch (ch)
		{
		case '+':						// add a new Child process
		{
			pid_t	pidChild;

			pidChild = createChild(indexChild, argv[1], dirChild);

			//=== child process or error ===
			if (pidChild == -1)
			{
				return 0;
			}
			//=== parent process ===
			struct Child* pChild = (Child*)malloc(sizeof(struct Child));

			pChild->index = indexChild;
			pChild->pid = pidChild;

			TAILQ_INSERT_TAIL(&head, pChild, allChildren);
			printf("Start Child: %d\n", indexChild);
			indexChild++;
			countChild++;

			break;
		}
		case '-':						// terminate latest created Child process
		{
			struct Child* pChild;

			pChild = TAILQ_LAST(&head, HeadOfCollection);
			if (pChild == NULL)
			{
				printf("There is no more Child elements\n");
				break;
			}
			printf("Terminate Child: %d\n", pChild->index);
			stopChild(pChild->pid);				// send signal to Child

			TAILQ_REMOVE(&head, pChild, allChildren);
			free(pChild);

			countChild--;
			printf("countChildren = %d\n", countChild);
			break;
		}
		case 'k':
		{
			stopChildAll();				// terminate all Child processes
			TAILQ_INIT(&head);
			break;
		}
		case 'g':
		{
			if (pChild != NULL)
			{
				startStatistic(pChild->pid);
			}
			else
			{
				startStatistic();
			}
			break;
		}
		case 's':
		{
			if (pChild != NULL)
			{
				stopStatistic(pChild->pid);
			}
			else
			{
				stopStatistic();
			}
			break;
		}
		case 'p':
		{
			if (pChild != NULL)
			{
				stopStatistic();				// запрещает всем C_k вывод
				startStatistic(pChild->pid);	// запрашивает C_<num> вывести свою статистику
			}
		}
		case 'l':
		{
			printf("Current state:\n");

			for (struct Child* pChild = head.tqh_first; pChild != NULL; )
			{
				printStatistic(pChild);
				pChild = pChild->allChildren.tqe_next;
			}
			break;
		}
		case 'q':
			stopChildAll();				// terminate all Child processes

			printf("main OK\n");
			return 0;

		case 'z':						// kill zombie processes
		{
			int stat;

			while (waitpid(-1, &stat, WUNTRACED) > 0)
			{
			}
			break;
		}

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

void startStatistic()
{
	for (struct Child* pChild = head.tqh_first; pChild != NULL; )
	{
		startStatistic(pChild->pid);
		pChild = pChild->allChildren.tqe_next;
	}
}

void startStatistic(pid_t pidChild)
{
	kill(pidChild, SIGUSR2);
}

void stopStatistic()
{
	for (struct Child* pChild = head.tqh_first; pChild != NULL; )
	{
		stopStatistic(pChild->pid);
		pChild = pChild->allChildren.tqe_next;
	}
}

void stopStatistic(pid_t pidChild)
{
	kill(pidChild, SIGUSR1);
}

void printStatistic(struct Child* pChild)
{
	char nameProgram[9] = { 0, };

	sprintf(nameProgram, "child_%02d", pChild->index);
	printf("\tnameProgram: %s\t", nameProgram);
	printf("pid: %d\n", pChild->pid);
}

struct Child* getChild(int indexChild)
{
	for (struct Child* pChild = head.tqh_first; pChild != NULL; )
	{
		if (pChild->index == indexChild)
		{
			return pChild;
		}
		pChild = pChild->allChildren.tqe_next;
	}
	return NULL;
}

void stopChild(pid_t pidChild)
{
	kill(pidChild, SIGKILL);
}

void stopChildAll()
{
	for (struct Child* pChild = head.tqh_first; pChild != NULL; )
	{
		struct Child* pChildNext;

		pChildNext = pChild->allChildren.tqe_next;

		printf("Terminate Child: %d\n", pChild->index);
		stopChild(pChild->pid);				// send signal to Child
		free(pChild);

		pChild = pChildNext;
	}
	countChild = 0;
}
