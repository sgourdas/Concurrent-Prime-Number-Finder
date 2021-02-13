#include "Functions.h"
// Make a pipe array based on children count
int **makePipeArray(int childrenCount) {

	int **fd = (int **) malloc(childrenCount * sizeof(int *));

	for(int i = 0 ; i < childrenCount ; i++)
		fd[i] = (int *) malloc(2 * sizeof(int)); 	// PUT THIS IN A FUNCTION

	return fd;

}
// Returns the num parameter as a heap allocated string
char *itoa2(int num) {
        
        char *str = (char *) malloc(20 * sizeof(char));
        sprintf(str, "%d", num);

        return str;

}
// Converts num to string and copies it to a pre-allocated space
void itoa3(int num, char *str) {
        
        sprintf(str, "%d", num);

}