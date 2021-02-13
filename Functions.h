#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h> // exec*
#include <signal.h>
#include <string.h>
#include <math.h>
#include <poll.h>
#include <errno.h>


#include "Types.h"

/* myprime.c */
void argCheck(int argc, char **argv);
void setupSigaction(void);
void createInnerLeafs(int argc, char **argv, int **fd);
void readInnerLeafs(List *primesList, List *timingsList, int **fd, int childrenNum);
/* innerLeafs.char */
void createWorkers(int, char **, int **);
List readWorkers(Info *, int **, int);
void writeRoot(List, Info *, int);
/* prime.c */
int prime (int);
double primeFirst(int, int);
int prime2(int);
double primeSecond(int, int);
double primeThird(int, int);
/* util.c */
int **makePipeArray(int);
char  *itoa2(int);
void itoa3(int, char *);
/* List.c */
List LCreate(Info);
void LInsert(List, Info);
void LPrintPrimes(List);
void LPrintTimings(List);
void LSortNumber(List);
void LSortTime(List);
Info LFirst(List);
Info LLast(List);
void LReverseNumbers(List);
void LDestroy(List);