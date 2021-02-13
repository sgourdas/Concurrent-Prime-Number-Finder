#include "Functions.h"

int main(int argc, char **argv) {	// Arguments = {minNum, maxNum, childrenNum, windex, rootPID}
	// Init childrenNum and pipe array
	int childrenNum = atoi(argv[3]), **fd = makePipeArray(childrenNum);
	// Create the workers
	createWorkers(argc, argv, fd);
	// Setup array used for children times and recognizing change of input from primes
	Info *childDone = (Info *) malloc(childrenNum * sizeof(Info));
	// Read info written from workers
	List list = readWorkers(childDone, fd, childrenNum);
	// Sort the workers' results based on primes
	LSortNumber(list);
	// Write results to the root
	writeRoot(list, childDone, childrenNum);
	// Free up space occupied
	for(int i = 0 ; i < childrenNum ; i++)
		free(fd[i]);

	free(fd);
	free(childDone);

	return 0;

}

void createWorkers(int argc, char **argv, int **fd) {

	int minNum = atoi(argv[1]), maxNum = atoi(argv[2]), childrenNum = atoi(argv[3]), iIndex = atoi(argv[4]), minRange, maxRange, algorithm, workerIndex, range = (int) (maxNum - minNum) / childrenNum;
	char *minRangeStr, *maxRangeStr, *algorithmStr, *workerIndexStr;
	pid_t *workerPIDS = (pid_t *) malloc(childrenNum * sizeof(pid_t));
	// For every child
	for(int i = 0 ; i < childrenNum ; i++) {
		// Setup a pipe
		if(pipe(fd[i]) < 0)
			exit(-1);
		// Fork process
		if((workerPIDS[i] = fork()) < 0)
			exit(-2);
		// Child execution
		if(workerPIDS[i] == 0) {	
			// Close reading end of pipe for child
			if(close(fd[i][0]) == -1)
			 	exit(-3);
			// Redirect pipe to stdout
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
			// Calculate the worker index and the algorithm it will use based on that -- homemade formulas :D
			workerIndex = iIndex * childrenNum + i;
			algorithm = workerIndex % 3 + 1;
			// Convert arguments to char* for argument passing
			maxRangeStr = itoa2(maxRange);
			minRangeStr = itoa2(minRange);
			algorithmStr = itoa2(algorithm);
			workerIndexStr = itoa2(workerIndex);
			// Overlay process with workers executable, passing the correct arguments
			execl("./workers", "./workers", minRangeStr, maxRangeStr, algorithmStr, argv[5], workerIndexStr, NULL);
			// This will only run if the exec fails
			exit(-4);

		}
		
	}

	free(workerPIDS);

}

List readWorkers(Info *childDone, int **fd, int childrenNum) {

	int childrenDoneCounter = 0, pollRes;
	List list = NULL;
	Info info;
	struct pollfd *fdtab = (struct pollfd *) malloc(childrenNum * sizeof(struct pollfd));
	// For every children init poll struct and childDone array
	for(int i = 0 ; i < childrenNum ; i++) {

		childDone[i].number = 0;
		fdtab[i].events = POLLIN;
		fdtab[i].fd = fd[i][0];

	}
	// Untill all children are done
	while(childrenDoneCounter != childrenNum) {
		// Cycle through children
		for(int i = 0 ; i < childrenNum ; i++) {
			// This means the initial value has been overwritten so this child is done, if true
			//fprintf(stderr, "check%d\n", i);
			if(childDone[i].number != 0)
				continue;
			// Poll pipe
			pollRes = poll(&fdtab[i], 1, 0);
			// fprintf(stderr, "poll = %d\n", p);
			// Take action based on poll result
			if(pollRes == 0 || (fdtab[i].revents & POLLIN) != POLLIN)	// Skip child if not ready
				continue;
			else if((pollRes < 0) && (errno != EINTR))	// Error
				exit(-3);
			// Read from pipe
			read(fd[i][0], &info, sizeof(info));
			//fprintf(stderr, "%d | %lf => %d\n", info.number, info.time, childrenDoneCounter);
			if(info.number < 0) {	// If info.number is <= 0, this means we have read all primes from the child -- info.time has the child's total time

				childDone[i] = info;	// Insert total time to the array with children times -- changes value from -1.0 signaling child is finished
				childrenDoneCounter++;	// Up the counter -- one more child is done
				//fprintf(stderr, "childrenCount: %d - childrenNum: %d!%d\n", childrenDoneCounter, childrenNum, i);
				continue;

			}
			// Insert info read to the list
			if(list == NULL)
				list = LCreate(info);
			else
				LInsert(list, info);


		}

	}
	// Close our end of the pipe
	for(int i = 0 ; i < childrenNum ; i++)
		close(fd[i][0]);
	// Free up allocated heap speace
	free(fdtab);
	// Return the list we created from reading workers' job
	return list;

}

void writeRoot(List temp, Info *childDone, int childrenNum) {

	Info buf[BUFFER_SIZE], term = {1, -1.0}; // term is the terminating node
	int i;

	while(temp != NULL) {	// Parse the list with the primes and timings until we reach the end
		// Copy items from list to buffer
		for(i = 0 ; (i < BUFFER_SIZE) && (temp != NULL) ; i++, temp = temp->link) {	// Copy until we fill the buffer or reach the end of the list 

			buf[i] = temp->info;

		}

		if(temp == NULL)	// If this gets triggered we break because we will write to the pipe after adding some info below
			break;

		write(STDOUT_FILENO, buf, sizeof(buf));

	}
	// Ended writing of primes to pipe
	// Here, we are left at a point in the buffer (i). We need to also write the timings of the lower levels to the pipe
	for(int child = 0 ; child < childrenNum ; child++, i++) {

		if(i == BUFFER_SIZE) {	// Edge case where we have reached the end of the buffer at the same time the list ended

			write(STDOUT_FILENO, buf, sizeof(buf));	// We write the buffer to the pipe so we get to set the terminating information in the next write
			
			buf[i = 0] = childDone[child];	// Reset i and write child time to buffer

		} else {
			
			buf[i] = childDone[child];	// Write child time to buffer

		}

	}

	if(i == BUFFER_SIZE) {	// Edge case where we have reached the end of the buffer at the same time the list ended

		// write(STDOUT_FILENO, buf, sizeof(buf));	// We write the buffer to the pipe so we get to set the terminating information in the next write
		write(STDOUT_FILENO, buf, sizeof(buf));
		buf[0] = term;	// Insert terminating character to the first index -- other indexes will be discarded on read

	} else {
			
		buf[i] = term;	// Insert terminating character

	}
	// Final write to input all information
	write(STDOUT_FILENO, buf, sizeof(buf));
	// Close our end of the pipe
	if(close(STDOUT_FILENO) == -1)
		exit(-3);

}