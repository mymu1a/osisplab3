#include "global.h"

#include <cstdio>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/queue.h>

//===== CHILD LIST =====
struct Child
{
	unsigned	index;
	pid_t		pid;

	TAILQ_ENTRY(Child) allChildren;         /* Tail queue. */
};

TAILQ_HEAD(HeadOfCollection, Child) head;

//===== functions =====
pid_t	pidChild_test = 0;

int createChild(unsigned indexChild, char* pathChildEnv, char* dirChild);
void printStatistic(struct Child* pChild);
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
	TAILQ_INIT(&head);                      /* Initialize the queue. */

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
			struct Child* pChild = (Child*)malloc(sizeof(struct Child));

			pChild->index = indexChild - 1;
			pChild->pid = pidChild;

			//TAILQ_INSERT_TAIL(&head, pChild, allChildren);

			///////////////////////////////////////////
			do {
					(pChild)->allChildren.tqe_next = NULL;
					(pChild)->allChildren.tqe_prev = (head).tqh_last;
					* (head).tqh_last = (pChild);
					(head).tqh_last = &(pChild)->allChildren.tqe_next;
			} while (0);
			///////////////////////////////////////////

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

		case '-':

		break;
		case 'l':
		{
			for (struct Child* pChild = head.tqh_first; pChild != NULL; )
			{
				printStatistic(pChild);
				//pChild = pChild->allChildren.tqe_next;
				(pChild) = (*(head).tqh_last);//////////////
			}
			///// begin
			struct Child* pChild;

/*			(pChild) = (*(head).tqh_last);
			printf("pChild = %p", pChild*/

			printf("head.tqh_first = %p\n", head.tqh_first);
			printf("head.tqh_last = %p\n", head.tqh_last);
			printf("*head.tqh_last = %p\n", *head.tqh_last);

//			printStatistic(head.tqh_first);
//			printStatistic(*head.tqh_last);
			pChild = (struct Child*)(*(head.tqh_last));
			//*
//			pChild = TAILQ_LAST(&head, Child);
			if (pChild != NULL)
			{
				printStatistic(pChild);
			}
			else
			{
				printf("not found\n");
			}
			//*/
			///// end
			break;
		}
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

void printStatistic(struct Child* pChild)
{
	printf("printStatistic ST\n");
	char nameProgram[9] = { 0, };


	sprintf(nameProgram, "child_%02d", pChild->index);
	printf("nameProgram: %s\t", nameProgram);
	printf("pid: %d\n", pChild->pid);
		
	printf("printStatistic OK\n");
}
