#define BUFFER_SIZE 256

typedef struct pack {

	int number;
	double time;

} Info;

typedef struct lst {

	Info info;
	struct lst *link;

} ListNode;

typedef ListNode *List;