#include "Functions.h"

#define YES 1
#define NO 0

int main (int argc , char **argv) {		// Arguments = {minNum, maxNum, algorithm, rootPID, workerIndex}

	int minNum = atoi(argv[1]), maxNum = atoi(argv[2]), algorithm = atoi(argv[3]), rootPID = atoi(argv[4]), workerIndex = atoi(argv[5]);
	Info breakPoint = {-(workerIndex + 1)};	// +1 is here to protect us, because we cant identify 0 as negative
	// Switch case to choose correct algorithm for the range given
	switch (algorithm) {

		case(1): {

			breakPoint.time = primeFirst(minNum, maxNum);

			break;

		} case(2): {

			breakPoint.time = primeSecond(minNum, maxNum);

			break;

		} case(3): {

			breakPoint.time = primeThird(minNum, maxNum);

			break;

		} default: {

			exit(-5);

		}

	}

	write(STDOUT_FILENO, &breakPoint, sizeof(breakPoint));
	// Setup and send SIGUS1 signal to root
	union sigval val;
	sigqueue(rootPID, SIGUSR1, val);
	// Close open pipe
	if(close(STDOUT_FILENO) == -1)
		exit(-3);

	return 0;

}

double primeFirst(int lb, int ub) {	// Method 1 for finding primes

	struct tms tb;
	double ticspersec = (double) sysconf(_SC_CLK_TCK), tStart = (double) times(&tb), t1 = tStart, t2, time;
	Info info;

	for(int i = lb ; i <= ub ; i++) {

		if(prime(i) == YES) {
			// i is prime here
			t2 = (double) times(&tb);
			// Get the time this prime took to calculate -- time since last prime/start
			time = (t2 - t1) / ticspersec;
			// Set variable t1 for next calculation -- latest prime time
			t1 = t2;
			// Fill the struct
			info.number = i;
			info.time = time;
	
			write(STDOUT_FILENO, &info, sizeof(info));

		}
	
	}
	// Get total runtime of algorithm
	return ((double) times(&tb)) - tStart;

}

int prime(int n) {	

	int i;

	if (n == 1) 
		return(NO);

	for(i = 2 ; i < n ; i++)
		if(n % i == 0) 
			return(NO);

	return(YES);

}

double primeSecond(int lb, int ub) {	// Method 2 for finding primes

	struct tms tb;
	double ticspersec = (double) sysconf(_SC_CLK_TCK), tStart = (double) times(&tb), t1 = tStart, t2, time;
	Info info;

	for(int i = lb ; i <= ub ; i++) {
		
		if(prime2(i) == YES) {
			// i is prime here
			t2 = (double) times(&tb);
			// Get the time this prime took to calculate -- time since last prime/start
			time = (t2 - t1) / ticspersec;
			// Set variable t1 for next calculation -- latest prime time
			t1 = t2;
			// Fill the struct
			info.number = i;
			info.time = time;

			write(STDOUT_FILENO, &info, sizeof(info));
		
		}

	}
	// Get total runtime of algorithm
	return ((double) times(&tb)) - tStart;

}

int prime2(int n) {

	int i = 0, limitup = 0;

	limitup = (int) (sqrt((float) n));

	if(n == 1) 
		return(NO);
	
	for(i = 2 ; i <= limitup ; i++)
		if (n % i == 0) 
			return(NO);
	
	return(YES);

}

double primeThird(int MINNUM, int MAXNUM) {	//complete

	struct tms tb;
	double ticspersec = (double) sysconf(_SC_CLK_TCK), tStart = (double) times(&tb), t1 = tStart, t2, time;
	int temp;
	Info info;

	if(MINNUM < 2) {	// Special case for if 2 is in our range, because the algorithm does not check even numbers
		// Fill struct
		info.number = 2;
		info.time = 0.0;
		// Write to pipe
		write(STDOUT_FILENO, &info, sizeof(info));

	}

    for (int i = MINNUM | 1 ; i <= MAXNUM ; i += 2 ) {

		if(((i % 3 != 0) && (i % 5 != 0) && (i % 7 != 0)) || (i == 3) || (i == 5) || (i == 7)) {

			temp = 0;

			for (int j = 5 ; j * j <= i ; j += 6) {

				if ( (i % j == 0) || (i % (j + 2) == 0) ) {

					temp++;		//If our number divides with another one in the range (1,i) then it is not a prime so we break the loop and move on
					break;

				}

			}

		    if (temp == 0) {
		    	// i is prime here
		   		t2 = (double) times(&tb);
				// Get the time this prime took to calculate -- time since last prime/start
				time = (t2 - t1) / ticspersec;
				// Set variable t1 for next calculation -- latest prime time
				t1 = t2;
				// Fill the struct
				info.number = i;
				info.time = time;
				// Write info (struct) to the pipe
				write(STDOUT_FILENO, &info, sizeof(info));

			}

		}

	}
	// Get total runtime of algorithm
	return ((double) times(&tb)) - tStart;

}