#include "Functions.h"


List LCreate(Info info) {	// List create
	// Allocate space and assign values
	List l = (List) malloc(sizeof(ListNode));
	l->info = info;
	l->link = NULL;

	return l;

}

void LInsert(List list, Info info) {	// List insertion

	// Allocate new space and assign values
	List l = LCreate(info);
	// Linkage
	l->link = list->link;
	list->link = l;

}

void LPrintPrimes(List list) {	// List printing for debugging

	while(list != NULL) { // Parse all nodes

		printf("%d %lf ", list->info.number, list->info.time);

		list = list->link;
	
	}

}

void LPrintTimings(List list) {	// List printing for debugging

	// Parse all nodes
	while(list != NULL) {

		printf("Time for W%d: %d msecs\n", list->info.number - 1, (int) list->info.time * 1000);

		list = list->link;
	
	}

}

void LSortNumber(List list) {

	int a;
    double a2;
    // For every node
    for(List temp1 = list ; temp1 != NULL ; temp1 = temp1->link) {
    	// For every node after the outer one
        for(List temp2 = temp1->link ; temp2 != NULL ; temp2 = temp2->link) {
        	// Sort based on number
            if(temp2->info.number < temp1->info.number) {
            	// Swap contents
                a = temp1->info.number;
                a2 = temp1->info.time;
                temp1->info.number = temp2->info.number;
                temp1->info.time = temp2->info.time;
                temp2->info.number = a;
                temp2->info.time = a2;

            }

        }

    }

}

void LSortTime(List list) {

	int a;
    double a2;
    // For every node
    for(List temp1 = list ; temp1 != NULL ; temp1 = temp1->link) {
    	// For every node after the outer one
        for(List temp2 = temp1->link ; temp2 != NULL ; temp2 = temp2->link) {
        	// Sort based on time
            if(temp2->info.time < temp1->info.time) {
            	// Swap contents
                a = temp1->info.number;
                a2 = temp1->info.time;
                temp1->info.number = temp2->info.number;
                temp1->info.time = temp2->info.time;
                temp2->info.number = a;
                temp2->info.time = a2;

            }

        }

    }

}

Info LFirst(List list) {
	// Return last node info
	return list->info;

}

Info LLast(List list) {
	// Get to the last node
	while(list->link != NULL)
		list = list->link;
	// Return last node info
	return list->info;

}

void LReverseNumbers(List list) {	// List printing for debugging

	
	while(list != NULL) { // Parse all nodes

		list->info.number = -list->info.number;	// Reverse values of numbers

		list = list->link;
	
	}

}

void LDestroy(List list) {	// Free space of a List 
	// Recursively free extra nodes
	if(list->link != NULL)
		LDestroy(list->link);
	// Free current node
	free(list);

}