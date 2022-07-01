/////////////////////// BT19CSE033 GOWTHAM GORREPATI /////////////////////
/////////////////////////// DSPD-2 ASSIGNMENT-1 /////////////////////////
///////////////////////////// HEADER FILE //////////////////////////////

#include <stdlib.h>
#include <string.h>
#define DB_SIZE 100
#define NAME_LEN 20
#define CLASS_LEN 20
#define STATUS_LEN 20
typedef enum{FAILURE, SUCCESS} status_code;
typedef enum{FALSE, TRUE} boolean;

typedef struct time_tag // assumed in 24 hour format
{
	unsigned int hour;
	unsigned int min;
} time;

// Node contains all the fields required for any kind of list
typedef struct Node_tag // CLL
{
	char flight_name[NAME_LEN];// ASSUMING NAME WITHOUT SPACES
	unsigned int flight_id;
	unsigned int flight_capacity;
	time arr_time;
	time dep_time;
	char flight_class[CLASS_LEN];// ASSUMING NAME WITHOUT SPACES

	unsigned int key;

	char flight_status[STATUS_LEN];// ASSUMING NAME WITHOUT SPACES
	time delay_time;

    struct Node_tag *next;
} Node;

typedef struct flight_DB_tag
{
	Node *last; // main list
	Node *vLast; // list for VIP
	Node *vvLast; // list for VVIP
	Node *pLast; // list for public
} flight_DB;

// Function Declarations
void initialize (flight_DB *flp);
Node *makeNode (char name[], unsigned int id, unsigned int cap, time arr, time dep, time delay_time, char fl_class[], unsigned int key);
Node *makeListNode (char name[], unsigned int id, time arr, char fl_status[], time delay_time);
Node* CL_InsertAtStart(Node* nptr, Node* last);
Node* CL_DeleteAtStart(Node *last);
Node *deleteNode (Node *start, Node *nptr);
void freeList (flight_DB *flp);

void printTime (unsigned int h, unsigned int m);
void visit (Node *nptr);
void printRec (flight_DB *flp, int select_flag);

boolean comparator (Node *aptr, Node *bptr, int compare_flag);
boolean isKeyEqual (Node *aptr, Node *bptr);
boolean isNewKeyEqual (Node *aptr, Node *bptr);
unsigned int computeKey (unsigned int id, time arr);
Node *selectLast (flight_DB *flp, int select_flag);
void updateLast (flight_DB *flp, Node *last, int select_flag);

Node* divide (Node* lptr);
Node* merge (Node *lptr, Node *nptr, int compare_flag);
Node* mergeSort (Node* list_ptr, int compare_flag);
void sort (flight_DB *flp, int compare_flag);

void update (Node *nptr, char name[], unsigned int cap, time dep, time delay_time, char fl_class[]);
status_code insertUpdateSubList (flight_DB *flp, char name[], unsigned int id, unsigned int cap, time arr, time del, time dep, char fl_class[]);
status_code insertUpdate (flight_DB *flp, char name[], unsigned int id, unsigned int cap, time arr, time dep, time delay_time, char fl_class[], int select_flag);
status_code deleteSubList (flight_DB *flp, unsigned int id);
status_code deleteList (flight_DB *flp, unsigned int id, int select_flag);
int getNumFlights (flight_DB *flp, int count_flag);
boolean isEmpty (flight_DB *flp);
boolean isFull (flight_DB *flp);
void getFlightWithLongestStay (flight_DB *flp);
void getSortedOnArrivalTime (flight_DB *flp);
void getSortedOnDepartureTime (flight_DB *flp);
void getSortedOnStayTime (flight_DB *flp);
void updateSubList (flight_DB *flp, unsigned int key, time delay_time, int select_flag);
void deleteFromSubList(flight_DB *flp, unsigned int key, int select_flag);
void updateStatus (flight_DB *flp, Node *ptr, Node *nptr);
status_code updateFlightStatus (flight_DB *flp, flight_DB *fl2);
void listUnique (flight_DB *flp);
