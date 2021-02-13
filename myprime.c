#include "Functions.h"

int USR1counter = 0;


int main(int argc, char **argv) {

	argCheck(argc, argv);
	// Setup signal handler for SIGUSR1
	setupSigaction();
	// Init parameters children count and pipe array
	int childrenNum = atoi(argv[6]), **fd = makePipeArray(childrenNum);
	// Create inner leafs
	createInnerLeafs(argc, argv, fd);
	// Init list pointers
	List *list = (List*) malloc(sizeof(List)), *timings = (List*) malloc(sizeof(List));
	// Read info from inner leafs and copy it to our list pointers
	readInnerLeafs(list, timings, fd, childrenNum);
	// Print primes
	LSortNumber(*list);
	printf("Primes in [%s, %s] are: ", argv[2], argv[4]);
	fflush(stdout);
	LPrintPrimes(*list);
	printf("\n");
	// Print mix and max time
	LSortTime(*timings);
	printf("Min Time for Workers: %d msecs\n", (int) LFirst(*timings).time * 1000);
	printf("Max Time for Workers: %d msecs\n", (int) LLast(*timings).time * 1000);
	// Print number os SIGUSR1 signals received
	printf("Num of USR1 Received: %d/%d\n", USR1counter, childrenNum * childrenNum);
	// Print worker times exactly
	LReverseNumbers(*timings);
	LSortNumber(*timings);
	LPrintTimings(*timings);
	// Free up space on heap
	LDestroy(*list);
	free(list);
	LDestroy(*timings);
	// Free up allocated space
	free(timings);

	for(int i = 0 ; i < childrenNum ; i++)
		free(fd[i]);

	free(fd);

	return 0;
	
}

void argCheck(int argc, char **argv) {

	int minRange = atoi(argv[2]), maxRange = atoi(argv[4]), childrenNum = atoi(argv[6]);

	if((argc != 7) || (minRange > maxRange) || (minRange < 1) || (maxRange - minRange < childrenNum * childrenNum)) {

		fprintf(stderr, "Invalid arguments.\n");
		exit(-5);

	}

}

void sigusr1_handler(int signum, siginfo_t *info, void *ucontext) {	// When recieving a SIGUSR1 signal we up our counter

	++USR1counter;

}

void setupSigaction(void) {

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = &sigusr1_handler;
	sa.sa_flags = SA_RESTART | SA_SIGINFO;	// SA_RESTART to protect from syscall interruptions
	
	if(sigaction(SIGUSR1, &sa, NULL) == -1)
		exit(1);

}

void createInnerLeafs(int argc, char **argv, int **fd) {

	int minNum = atoi(argv[2]), maxNum = atoi(argv[4]), childrenNum = atoi(argv[6]), minRange, maxRange, range = (int) (maxNum - minNum) / childrenNum;
	char *minRangeStr, *maxRangeStr, *indexStr, rootPID[32];
	itoa3(getpid(), rootPID);
	pid_t *innerLeafPIDS = (pid_t *) malloc(childrenNum * sizeof(pid_t));
	// For each child
	for(int i = 0 ; i < childrenNum ; i++) {
		// Setup a pipe
		if(pipe(fd[i]) < 0)
			exit(-1);
		// Fork process
		if((innerLeafPIDS[i] = fork()) < 0)
			exit(-2);
		// Child execution
		if(innerLeafPIDS[i] == 0) {	
			// Close reading end of pipe for child
			if(close(fd[i][0]) == -1)
			 	exit(-3);

			dup2(fd[i][1], 1);
			// Calculate range to pass to executable
			if(i == 0)
				minRange = minNum;
			else
				minRange = minNum + range * i + 1;
			// If on last child (last loop), set max to maxnum;
			if(i == childrenNum - 1)
				maxRange = maxNum;
			else
				maxRange = minNum + range * (i + 1);
			// Convert arguments to char* for argument passing
			maxRangeStr = itoa2(maxRange);
			minRangeStr = itoa2(minRange);
			indexStr = itoa2(i);
			// Overlay process with primes executable, passing the correct arguments
			execl("./innerLeafs", "./innerLeafs", minRangeStr, maxRangeStr, argv[6], indexStr, rootPID, NULL);	
			// This will only run if the exec fails
			exit(-4);

		}
		
	}

	for(int i = 0 ; i < childrenNum ; i++)	// Close write pipe ends
		close(fd[i][1]);

	free(innerLeafPIDS);

}

void readInnerLeafs(List *primesList, List *timingsList, int **fd, int childrenNum) {

	int childrenDoneCounter = 0, *childDone = (int *) malloc(childrenNum * sizeof(int)), pollRes;
	List list = NULL, timings = NULL;
	Info buf[BUFFER_SIZE];
	struct pollfd *fdtab = (struct pollfd *) malloc(childrenNum * sizeof(struct pollfd));
	// For every children init poll struct and childDone array
	for(int i = 0 ; i < childrenNum ; i++) {

		childDone[i] = -1;
		fdtab[i].events = POLLIN;
		fdtab[i].fd = fd[i][0];

	}
	// Loop until all children have ended
	while(childrenDoneCounter != childrenNum) {
		// Cycle through children
		for(int i = 0 ; i < childrenNum ; i++) {
			// If initial value has been changed, this child is done
			if(childDone[i] != -1)
				continue;
			// Poll pipe
			pollRes = poll(&fdtab[i], 1, 0);
			// Take action based on poll result
			if(pollRes == 0 || (fdtab[i].revents & POLLIN) != POLLIN)
				continue;
			else if((pollRes < 0) && (errno != EINTR))
				exit(-3);
			// Read a buf from pipe
			read(fd[i][0], buf, sizeof(buf));
			// Check if child is done
			for(int j = 0 ; j < BUFFER_SIZE ; j++) {	
				// If the number is negative we have reached end of primes -- recognize that and take action
				if(buf[j].number < 0) {

					childDone[i] = j;	// Marks the end of the primes the last buffer we read - for the i-th child
					childrenDoneCounter++;

					break;

				}

			}

			int j = 0;

			if(!list)	// List is not initialized
				list = LCreate(buf[j++]);	// Init list

			if(list) {	// List initialized

				while(j < childDone[i]) {	// Until terminating primes condition

					LInsert(list, buf[j++]);	// Insert primes and their time to list

				}
			
			}
			// Copy timings over to the timings list
			if(childDone[i] != BUFFER_SIZE) {
				// Set starting index, saved before
				j = childDone[i];

				if(!timings)	// timings list not initialized
					timings = LCreate(buf[j++]);

				while(1) {

					if(j == BUFFER_SIZE) {

						read(fd[i][0], buf, sizeof(buf));	// Read from pipe

						j = 0;	// Reset index

					}

					if(buf[j].number == 1)	// break when we find the terminating character
						break;

					LInsert(timings, buf[j++]);	// Insert info to timings list

				}

			}

		}

		

	}
	// Free up allocated heap space
	free(fdtab);
	free(childDone);
	// Assign lists
	*primesList = list;
	*timingsList = timings;

}