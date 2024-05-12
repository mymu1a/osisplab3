#include "global.h"

#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/queue.h>
#include <sys/wait.h>

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
void onExit();										// clear resources
void printStatistic(struct Child* pChild);
void printUsage(char* nameProgram, short index);
void showStatisticAfterTimer();
void stopChild(pid_t pidChild);						// send `SIGKILL` signal to Child
void stopChildAll();								// terminate all Child processes
void startStatistic();
void startStatistic(pid_t pidChild);
void stopStatistic();
void stopStatistic(pid_t pidChild);

//=== Global Variables ===
TAILQ_HEAD(HeadOfCollection, Child) head = TAILQ_HEAD_INITIALIZER(head);
unsigned countChild = 0;
timer_t	idTimer = 0;
int gFlag = 0;

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

//	while ((ch = getchar()) != EOF)
	while (true)
	{
		///// begin
		ch = getchar();		///
		if (ch == EOF)
		{
			printf("EOF");
			continue;
		}
		///// end
		int charNext;
		struct Child* pChild = NULL;

		if (ch == 's' || ch == 'g' || ch == 'p')
		{
			charNext = getchar();
			if (isdigit(charNext))
			{
				pChild = getChild(charNext - 0x30);
			}
			if(pChild == NULL)
			{ // return 'char' back to the stream
				ungetc(charNext, stdin);
			}
		}
		//=== select command ===
		switch (ch)
		{
		case '+':								// add a new Child process
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
		case '-':								// terminate latest created Child process
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
		case 'g':								// show Statistic
		{
			gFlag = 1;
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
		case 'k':								// terminate all Child processes
		{
			stopChildAll();
			TAILQ_INIT(&head);
			break;
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
		case 'p':								// print Statistic for single Child only
		{
			if (pChild != NULL)
			{
				stopStatistic();				// запрещает всем C_k вывод
				usleep(1000);
				startStatistic(pChild->pid);	// запрашивает C_<num> вывести свою статистику
				showStatisticAfterTimer();
			}
			break;
		}
		case 'q':
		{
			onExit();							// clear resources

			printf("main OK quit\n");
			return 0;
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
	printf("main OK main");
	onExit();							// clear resources

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

// catch SIGUSR2 and send SIGUSR2 to all children
void signalHandler(int numberSignal)
{
	printf("signalHandler STOK\n");
	if (gFlag != 1)
	{
		startStatistic();
	}
}

// start signal timer
// set signal handler
void showStatisticAfterTimer()
{
	printf("showStatisticAfterTimer ST\n");

	if (idTimer == 0)
	{
		struct sigevent	event;

		event.sigev_notify = SIGEV_SIGNAL;
		event.sigev_signo = SIGUSR2;
		event.sigev_value.sival_ptr = NULL;

		if (timer_create(CLOCK_REALTIME, &event, &idTimer))
		{
			printf("Error during Timer creation: %s\n", strerror(errno));
			printf("showStatisticAfterTimer OK\n");
			exit(EXIT_FAILURE);
		}
		//=== connect to SIGUSR2 ===
		int					result;
		struct sigaction	action;

		action.sa_handler = signalHandler;
		action.sa_flags = 0;

		result = sigaction(SIGUSR2, &action, NULL);						// SIGUSR2
		if (result == -1)
		{
			printf("cannot connect to Signal: SIGUSR2\n");
			exit(EXIT_FAILURE);
		}
	}
	struct itimerspec	ts = { 0, };
	
	gFlag = 0;
	ts.it_value.tv_sec = 5;
	/*
	ts.it_value.tv_nsec = 1000;
	ts.it_interval.tv_sec = 0;
	ts.it_interval.tv_nsec = 0;
	//*/
	if (timer_settime(idTimer, 0/*TIMER_RELTIME*/, &ts, NULL) == -1)
	{
		printf("Error during Timer start: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("showStatisticAfterTimer OK\n");
}

// clear resources
void onExit()
{
	if (idTimer != 0)
	{
		timer_delete(idTimer);
		idTimer = 0;
	}
	stopChildAll();						// terminate all Child processes
}
