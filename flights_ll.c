/////////////////////// BT19CSE033 GOWTHAM GORREPATI /////////////////////
/////////////////////////// DSPD-2 ASSIGNMENT-1 /////////////////////////
//////////////////////////   ASSUMPTIONS MADE   /////////////////////////
/*
	> TIME is in 24 hour format.
	> Strings are inputted without spaces.

	> FLIGHT ID is a non-zero integer <=9999.
	> Each flight record will have a single class (VIP or VVIP or public).
	> DEPARTURE TIME is greater than ARRIVAL TIME for any particular record and a flight arrives and departures on the same day.
	> KEY is taken as a combination of FLIGHT ID and ARRIVAL TIME (giving preference to FLIGHT ID). (except for listUnique)
	
	> In listUnique function, key is the combination of Name and ID (giving preference to Name).
	> In the updateFlightStatus function, the combination of ID and ARRIVAL TIME are considered as key.
	> We assume that every record in the list2 of updateFlightStatus is present in the main list.
	> In the updateFlightStatus function, I have assumed that PUBLIC can be only promoted to VIP and VIP to only VVIP.
	> Please give length of name < 15 for the output table to not get disturbed.
	> I have used few functions for Singly Linked List (converted from Circular to Singly and back to Circular after operation).
	> I have focused on reusability of functions rather than space, so I have used the same struct flight_DB for all kinds of lists.
	> I have used comparator function to compare appropriate fields of a record.
	> select_flag will select proper list as per requirement.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BT19CSE033_DSPD-2_ASSIGNMENT-1.h"

void initialize (flight_DB *flp)
{
	flp->last = NULL;
	flp->vLast = NULL;
	flp->vvLast = NULL;
	flp->pLast = NULL;
}

Node *makeNode (char name[], unsigned int id, unsigned int cap, time arr, time dep, time delay_time, char fl_class[], unsigned int key)
{
	Node *nptr;
	nptr = (Node *)malloc(sizeof(Node));
	if(nptr != NULL)
	{
		strcpy(nptr->flight_name, name);
		nptr->flight_id = id;
		nptr->flight_capacity = cap;
		nptr->arr_time.hour = arr.hour;
		nptr->arr_time.min = arr.min;
		nptr->dep_time.hour = dep.hour;
		nptr->dep_time.min = dep.min;
		nptr->delay_time.hour = delay_time.hour;
		nptr->delay_time.min = delay_time.min;
		strcpy(nptr->flight_class, fl_class);
		nptr->flight_status[0] = '\0';
		nptr->key = key;
	}
	return nptr;
}

Node *makeListNode (char name[], unsigned int id, time arr, char fl_status[], time delay_time)
{
	Node *nptr;
	nptr = (Node *)malloc(sizeof(Node));
	if(nptr != NULL)
	{
		strcpy(nptr->flight_name, name);
		nptr->flight_id = id;
		nptr->arr_time.hour = arr.hour;
		nptr->arr_time.min = arr.min;
		nptr->delay_time.hour = delay_time.hour;
		nptr->delay_time.min = delay_time.min;
		strcpy(nptr->flight_status, fl_status);
		nptr->key = computeKey(id, arr);
	}
	return nptr;
}

Node* CL_InsertAtStart (Node* nptr, Node* last)
{
	if(last == NULL)
	{
		last = nptr;
		nptr->next = nptr;
	}
	else
	{
		nptr->next = last->next;
		last->next = nptr;
	}
	return last;
}

Node* CL_DeleteAtStart(Node *last)
{
    Node* first;
    if(last != NULL)
    {
        if(last->next == last)
        {
            free(last);
            last = NULL;
        }
        else
        {
            first = last->next;
            last->next = first->next;
            free(first);
        }
    }
    return last;
}

Node *deleteNode (Node *start, Node *nptr) // for SLL
{
	// assuming nptr is present in list
	Node *ptr;
	if(start == nptr)
	{
		ptr = start->next;
		free(start);
		start = ptr;
	}
	else
	{
		int done = 0;
		ptr = start;
		while(!done && ptr->next->next != NULL)
		{
			if(ptr->next == nptr)
			{
				ptr->next = nptr->next;
				free(nptr);
				done = 1;
			}
			ptr = ptr->next;
		}
		if(!done)
		{
			free(ptr->next);
			ptr->next = NULL;
		}
	}
	return start;
}

void freeList (flight_DB *flp)
{
	Node *ptr, *last;
	int i=0;
	for(i=0; i<4; i++)
	{
		last = selectLast(flp, i); // deleting all lists of a database
		if(last != NULL)
		{
			while(last != NULL)
			{
				last = CL_DeleteAtStart(last);
			}
		}
		updateLast(flp, last, i);
	}
}


void printTime (unsigned int h, unsigned int m)
{
	if(h < 10)
		printf("0%u:",h);
	else
		printf("%u:",h);
	if(m < 10)
		printf("0%u",m);
	else
		printf("%u",m);
}

void visit (Node *nptr)
{
	printf("\t\t%s\t\t%u\t%u\t\t", nptr->flight_name, nptr->flight_id, nptr->flight_capacity);
	printTime(nptr->arr_time.hour, nptr->arr_time.min);
	printf("\t\t");
	printTime(nptr->dep_time.hour, nptr->dep_time.min);
	printf("\t\t%s\t", nptr->flight_class);
	printTime(nptr->delay_time.hour, nptr->delay_time.min);
	printf("\n");
}

void printRec (flight_DB *flp, int select_flag)
{
	Node *nptr, *last;
	last = selectLast(flp, select_flag);
	if(last == NULL)
	{
		printf("\nList is empty.\n\n");
	}
	else
	{
		nptr = last->next;
		int i=1;
		printf("SERIAL NO.\tFLIGHT NAME\tID\tCAPACITY\tARRIVAL TIME\tDEPARTURE TIME\tCLASS\tDELAY TIME");
		do {
			printf("\n%d", i);
			i++;
            visit(nptr);
            nptr = nptr->next;
        } while(nptr != last->next);
	}
	updateLast(flp, last, select_flag);
}


 // returns true if required contents of aptr < that of bptr
boolean comparator (Node *aptr, Node *bptr, int compare_flag)
{
	boolean ret_val = TRUE;
	switch(compare_flag) {
		case 0: { // key
					if(aptr->flight_id > bptr->flight_id)
						ret_val = FALSE;
					else if(aptr->flight_id == bptr->flight_id)
					{
						if(comparator(aptr, bptr, 1) == FALSE)
							ret_val = FALSE;
					}
				}
				break;
		case 1: { // arrival time
					if(aptr->arr_time.hour > bptr->arr_time.hour)
						ret_val = FALSE;
					else if(aptr->arr_time.hour == bptr->arr_time.hour && aptr->arr_time.min >= bptr->arr_time.min)
						ret_val = FALSE;
				}
				break;
		case 2: { // departure time
					if(aptr->dep_time.hour > bptr->dep_time.hour)
						ret_val = FALSE;
					else if(aptr->dep_time.hour == bptr->dep_time.hour && aptr->dep_time.min >= bptr->dep_time.min)
						ret_val = FALSE;
				}
				break;
		case 3: { // stay time
					if(aptr->dep_time.hour-aptr->arr_time.hour > bptr->dep_time.hour-bptr->arr_time.hour)
						ret_val = FALSE;
					else if(aptr->dep_time.hour-aptr->arr_time.hour == bptr->dep_time.hour-bptr->arr_time.hour)
					{
						if(aptr->dep_time.min-aptr->arr_time.min >= bptr->dep_time.min-bptr->arr_time.min)
							ret_val = FALSE;
					}
				}
				break;
		case 4: { // delay time
					if(aptr->delay_time.hour > bptr->delay_time.hour)
						ret_val = FALSE;
					else if(aptr->delay_time.hour == bptr->delay_time.hour && aptr->delay_time.min >= bptr->delay_time.min)
						ret_val = FALSE;
				}
				break;
		case 5: { // for listUnique, Name and ID
					if(strcmp(aptr->flight_name,bptr->flight_name) > 0)
						ret_val = FALSE;
					else if(strcmp(aptr->flight_name,bptr->flight_name) == 0)
					{
						if(aptr->flight_id >= bptr->flight_id)
							ret_val = FALSE;
					}
				}
				break;
		default : break;
	}
	return ret_val;
}

boolean isKeyEqual (Node *aptr, Node *bptr)
{
	boolean ret_val = FALSE;
	if(aptr->flight_id == bptr->flight_id && aptr->arr_time.hour == aptr->arr_time.hour && aptr->arr_time.min == aptr->arr_time.min)
		ret_val = TRUE;
	return ret_val;
}

boolean isNewKeyEqual (Node *aptr, Node *bptr)
{
	// for listUnique
	boolean ret_val = FALSE;
	if(strcmp(aptr->flight_name, bptr->flight_name) == 0 && aptr->flight_id == bptr->flight_id)
		ret_val = TRUE;
	return ret_val;
}

unsigned int computeKey (unsigned int id, time arr)
{
	unsigned int key;
	key = 10000*id + 100*arr.hour + arr.min;
	return key;
}

Node *selectLast (flight_DB *flp, int select_flag)
{
	// selects appropriate list
	Node *last;
	switch (select_flag)
	{
		case 0: last = flp->last;
				break;
		case 1: last = flp->vLast;
				break;
		case 2: last = flp->vvLast;
				break;
		case 3: last = flp->pLast;
				break;
		default : last = flp->last;
				break;
	}
	return last;
}

void updateLast (flight_DB *flp, Node *last, int select_flag)
{
	// updates appropriate list
	switch (select_flag)
	{
		case 0: flp->last = last;
				break;
		case 1: flp->vLast = last;
				break;
		case 2: flp->vvLast = last;
				break;
		case 3: flp->pLast = last;
				break;
		default : flp->last = last;
				break;
	}
}


Node* divide (Node* lptr)
{
    Node *nptr, *fast, *slow;
    slow = lptr;
    fast = lptr->next->next;
    while(fast != NULL)
    {
        fast = fast->next;
        slow = slow->next;
        if(fast != NULL)
        {
            fast = fast->next;
        }
    }
    nptr = slow->next;
    slow->next = NULL;
    return nptr;
}

Node* merge (Node *lptr, Node *nptr, int compare_flag)
{
    Node *result, *ptr1, *ptr2, *tail;
    ptr1 = lptr;
    ptr2 = nptr;
    if(comparator(ptr1, ptr2, compare_flag) == TRUE)
    {
        result = ptr1;
        ptr1 = ptr1->next;
    }
    else
    {
        result = ptr2;
        ptr2 = ptr2->next;
    }
    tail = result;
    while(ptr1 != NULL && ptr2 != NULL)
    {
        if(comparator(ptr1, ptr2, compare_flag) == TRUE)
        {
            tail = tail->next = ptr1;
            ptr1 = ptr1->next;
        }
        else
        {
            tail = tail->next = ptr2;
            ptr2 = ptr2->next;
        }
    }
    if(ptr1 != NULL)
        tail->next = ptr1;
    else
        tail->next = ptr2;
    return result;
}

Node* mergeSort (Node* list_ptr, int compare_flag) // merge sort for SLL
{
    Node *nptr, *lptr;
    lptr = list_ptr;
    if((lptr != NULL) && (lptr->next != NULL))
    {
        nptr = divide(lptr);
        lptr = mergeSort(lptr, compare_flag);
        nptr = mergeSort(nptr, compare_flag);
        lptr = merge(lptr, nptr, compare_flag);
    }
    return lptr;
}

// sorting based on comparator
void sort (flight_DB *flp, int compare_flag)
{
	Node *store, *last;
	last = flp->last;
	if(last != NULL)
	{
		store = last->next;
		last->next = NULL; // changing to SLL
		store = mergeSort(store, compare_flag);
		Node *ptr = store;
		while(ptr != NULL)
		{
			last = ptr;
			ptr = ptr->next;
		}
		last->next = store;
		flp->last = last;
	}
	return ;
}


void update (Node *nptr, char name[], unsigned int cap, time dep, time delay_time, char fl_class[])
{
	// updates if keys are same
	strcpy(nptr->flight_name, name);
	nptr->flight_capacity = cap;
	nptr->dep_time.hour = dep.hour;
	nptr->dep_time.min = dep.min;
	nptr->delay_time.hour = delay_time.hour;
	nptr->delay_time.min = delay_time.min;
	strcpy(nptr->flight_class, fl_class);
}

status_code insertUpdateSubList (flight_DB *flp, char name[], unsigned int id, unsigned int cap, time arr, time dep, time delay_time, char fl_class[])
{
	status_code sc = SUCCESS;
	if(strcmp(fl_class, "VIP") == 0)
		sc = insertUpdate(flp, name, id, cap, arr, dep, delay_time, fl_class, 1);
	else if(strcmp(fl_class, "VVIP") == 0)
		sc = insertUpdate(flp, name, id, cap, arr, dep, delay_time, fl_class, 2);
	else if(strcmp(fl_class, "public") == 0)
		sc = insertUpdate(flp, name, id, cap, arr, dep, delay_time, fl_class, 3);
	else
		sc = FAILURE;
	return sc;
}

// this function updates the list in a sorted manner in every call
status_code insertUpdate (flight_DB *flp, char name[], unsigned int id, unsigned int cap, time arr, time dep, time delay_time, char fl_class[], int select_flag)
{
	// list is already sorted
	status_code sc = SUCCESS;
	unsigned int key = computeKey(id, arr);
	Node *last, *nptr, *ptr;
	last = selectLast(flp, select_flag);
	ptr = last;
	if(ptr == NULL) // empty list case
	{
		nptr = makeNode(name, id, cap, arr, dep, delay_time, fl_class, key);
		if(nptr!=NULL && getNumFlights(flp, select_flag)<=DB_SIZE)
		{
			last = nptr;
			last->next = last;
		}
		else
			sc = FAILURE;
	}
	else
	{
		ptr = last->next;
		if(ptr->next == ptr) // single node
		{
			if(ptr->key == key)
			{
				update(ptr, name, cap, dep, delay_time, fl_class);
			}
			else
			{
				nptr = makeNode(name, id, cap, arr, dep, delay_time, fl_class, key);
				if(nptr!=NULL && getNumFlights(flp, select_flag)<=DB_SIZE)
				{
					ptr->next = nptr;
					nptr->next = ptr;
					if(ptr->key < key)
						last = nptr;
					else
						last = ptr;
				}
				else
					sc = FAILURE;
			}
		}
		else
		{
			if(key < ptr->key) // need to insert at start
			{
				nptr = makeNode(name, id, cap, arr, dep, delay_time, fl_class, key);
				if(nptr!=NULL && getNumFlights(flp, select_flag)<=DB_SIZE)
				{
					nptr->next = ptr;
					last->next = nptr;
				}
				else
					sc = FAILURE;
			}
			else if(key > last->key) // need to insert at end
			{
				nptr = makeNode(name, id, cap, arr, dep, delay_time, fl_class, key);
				if(nptr!=NULL && getNumFlights(flp, select_flag)<=DB_SIZE)
				{
					nptr->next = ptr;
					last->next = nptr;
					last = nptr;
				}
				else
					sc = FAILURE;
			}
			else
			{
				int done = 0;
				if(ptr->key == key) // upda at first node
				{
					update(ptr, name, cap, dep, delay_time, fl_class);
				}
				else
				{
					Node *store = ptr;
					if(!(key < ptr->next->key))
						ptr = ptr->next;
					while(ptr != last->next && ptr->key < key)
					{
						store = ptr;
						ptr = ptr->next;
					}
					if(ptr->key == key)
					{
						update(ptr, name, cap, dep, delay_time, fl_class);
					}
					else // insert
					{
						ptr = store;
						nptr = makeNode(name, id, cap, arr, dep, delay_time, fl_class, key);
						if(nptr!=NULL && getNumFlights(flp, select_flag)<=DB_SIZE)
						{
							// insert node before
							nptr->next = ptr->next;
							ptr->next = nptr;
						}
						else
							sc = FAILURE;
					}
				}
			}
		}
	}
	updateLast(flp, last, select_flag);
	return sc;
}

status_code deleteSubList (flight_DB *flp, unsigned int id)
{
	status_code sc = SUCCESS;
	sc = deleteList(flp, id, 1);
	sc = deleteList(flp, id, 2);
	sc = deleteList(flp, id, 3);
	return sc;
}

status_code deleteList (flight_DB *flp, unsigned int id, int select_flag)
{
	// will delete records from start node to end node after finding them
	status_code sc = SUCCESS;
	Node *last, *nptr, *ptr;
	last = selectLast(flp, select_flag);
	ptr = last;
	if(ptr == NULL) // empty list case
	{
		sc = FAILURE;
	}
	else
	{
		ptr = last->next;
		if(ptr->next == ptr) // single node
		{
			if(ptr->flight_id == id)
			{
				free(ptr);
				last = NULL;
			}
			else
				sc = FAILURE;
		}
		else
		{
			Node *start, *end;
			ptr = last->next;
			Node *store = ptr;
			if(ptr->flight_id == id) // if last->next == start
			{
				start = ptr;
				end = ptr;
				ptr = ptr->next;
				while(ptr != last->next && ptr->flight_id == id)
				{
					store = ptr;
					ptr = ptr->next;
				}
				if(store == last) // deleting whole list
				{
					ptr = start;
					end = store;
					end->next = NULL; // making singly LL
					Node *next = ptr->next;
					while(ptr != NULL)
					{
						if(ptr != NULL) next = ptr->next;
						free(ptr);
						ptr = next;
					}
					last = NULL;
				}
				else
				{
					end = store;
					last->next = end->next;
					end->next = NULL; // changing to singly LL
					ptr = start;
					Node *next = ptr->next;
					while(ptr != NULL)
					{
						if(ptr != NULL) next = ptr->next;
						free(ptr);
						ptr = next;
					}
				}
			}
			else
			{
				int done = 0;
				ptr = ptr->next;
				while(ptr != last->next && !done) // search for start
				{
					if(ptr->flight_id == id)
						done = 1;
					else
						ptr = ptr->next;
				}
				if( !done )
					sc = FAILURE;
				else
				{
					start = ptr;
					while(ptr->flight_id == id) // definitely fails at ptr = start
					{
						end = ptr;
						ptr = ptr->next;
					}
					Node *travel;
					travel = last->next;
					while(travel->next != start)
						travel = travel->next;
					// now, travel->next == start
					if(end == last)
					{
						store = last->next;
						last = travel;
						last->next = store;
					}
					else
					{
						travel->next = end->next;
					}
					// delete from start to end
					end->next = NULL;
					ptr = start;
					Node *next = ptr->next;
					while(ptr != NULL)
					{
						if(ptr != NULL) next = ptr->next;
						free(ptr);
						ptr = next;
					}
				}
			}
		}
	}
	updateLast(flp, last, select_flag);
	return sc;
}

int getNumFlights (flight_DB *flp, int count_flag)
{
	int count = 0;
	Node *last, *ptr;
	last = selectLast(flp, count_flag);
	if(last != NULL)
	{
		count = 1;
		ptr = last->next->next;
		while(ptr != last->next)
		{
			ptr = ptr->next;
			count++;
		}
	}
	return count;
}

boolean isEmpty (flight_DB *flp)
{
	boolean ret_val = TRUE;
	if( flp->last != NULL ) {
		ret_val = FALSE;
	}
	return ret_val;
}

boolean isFull (flight_DB *flp)
{
	boolean ret_val = FALSE;
	if(getNumFlights(flp, 0) == DB_SIZE)
		ret_val = TRUE;
	return ret_val;
}

void getFlightWithLongestStay (flight_DB *flp)
{
	// print longest stay time also
	int n = getNumFlights(flp, 0);
	if(n>0)
	{
		int i=0, diff[DB_SIZE]={0}, hour_diff=0, max=0;
		Node *ptr, *last, *store;
		last = selectLast(flp, 0);
		store = last->next;
		last->next = NULL; // making SLL
		ptr = store;
		while (ptr != NULL)
		{
			hour_diff = ptr->dep_time.hour - ptr->arr_time.hour;
			if(ptr->dep_time.min < ptr->arr_time.min)
			{
				hour_diff--;
				diff[i] = 60;
			}
			diff[i] += 100*(hour_diff) + (ptr->dep_time.min-ptr->arr_time.min);
			i++;
			ptr = ptr->next;
		}
		for(i=0; i<n; i++)
		{
			if(diff[i]>max)
				max=diff[i];
		}
		i=0;
		ptr = store;
		printf("Longest stay time is : ");
		printTime(max/100, max%100);
		printf("\n");
		while (ptr != NULL)
		{
			if(diff[i] == max)
			{
				printf("Flight id : %u , Arrival Time : ",ptr->flight_id);
				printTime(ptr->arr_time.hour, ptr->arr_time.min);
				printf("\n");
			}
			i++;
			ptr = ptr->next;
		}
		last->next = store;
		updateLast(flp, last, 0);
	}
}

void getSortedOnArrivalTime (flight_DB *flp)
{
	sort(flp, 1);
	return ;
}

void getSortedOnDepartureTime (flight_DB *flp)
{
	sort(flp, 2);
	return ;
}

void getSortedOnStayTime (flight_DB *flp)
{
	sort(flp, 3);
	return ;
}

void updateSubList (flight_DB *flp, unsigned int key, time delay_time, int select_flag)
{
	Node *last = selectLast(flp, select_flag);
	Node *start, *ptr;
	start = last->next;
	last->next = NULL;
	ptr = start;
	int done = 0;
	while(ptr != NULL && !done)
	{
		if(ptr->key == key)
		{
			ptr->delay_time.hour = delay_time.hour;
			ptr->delay_time.min = delay_time.min;
			done = 1;
		}
		ptr = ptr->next;
	}
	last->next = start;
	updateLast(flp, last, select_flag);
}

void deleteFromSubList (flight_DB *flp, unsigned int key, int select_flag)
{
	Node *last = selectLast(flp, select_flag);
	Node *start = last->next;
	last->next = NULL;
	Node *ptr = start;
	int done = 0;
	while(ptr != NULL && !done)
	{
		if(ptr->key == key)
		{
			start = deleteNode(start, ptr);
			done = 1;
		}
	}
	ptr = start;
	while(ptr->next != NULL)
		ptr = ptr->next;
	last = ptr;
	last->next = start;
	updateLast(flp, last, select_flag);
}

void updateStatus (flight_DB *flp, Node *ptr, Node *nptr)
{
	status_code sc = SUCCESS;
	if(strcmp(nptr->flight_status, "DELAY") == 0)
	{
		ptr->delay_time.hour = nptr->delay_time.hour;
		ptr->delay_time.min = nptr->delay_time.min;
		if(strcmp(ptr->flight_class, "VIP") == 0)
			updateSubList(flp, ptr->key, nptr->delay_time, 1);
		else if(strcmp(ptr->flight_class, "VVIP") == 0)
			updateSubList(flp, ptr->key, nptr->delay_time, 2);
		else if(strcmp(ptr->flight_class, "public") == 0)
			updateSubList(flp, ptr->key, nptr->delay_time, 3);
	}
	else if(strcmp(nptr->flight_status, "PROMOTED") == 0)
	{
		if(strcmp(ptr->flight_class, "public") == 0)
		{
			strcpy(ptr->flight_class, "VIP");
			sc = insertUpdate(flp, ptr->flight_name, ptr->flight_id, ptr->flight_capacity, ptr->arr_time, ptr->dep_time, ptr->delay_time, ptr->flight_class, 2);
			deleteFromSubList(flp, ptr->key, 3);
		}
		else if(strcmp(ptr->flight_class, "VIP") == 0)
		{
			strcpy(ptr->flight_class, "VVIP");
			sc = insertUpdate(flp, ptr->flight_name, ptr->flight_id, ptr->flight_capacity, ptr->arr_time, ptr->dep_time, ptr->delay_time, ptr->flight_class, 1);
			deleteFromSubList(flp, ptr->key, 2);
		}
	}
}

status_code updateFlightStatus (flight_DB *flp, flight_DB *fl2)
{
	// assuming each element in list_2 is present in main list
	status_code sc = SUCCESS;
	Node *ptr, *nptr, *start, *head, *lptr, *last;
	sort(fl2, 0); // sort based on key for O(n) update in next step
	last = flp->last;
	lptr = fl2->last;
	start = last->next;
	head = lptr->next;
	last->next = NULL;
	lptr->next = NULL;
	ptr = start; // main list
	nptr = head; // list 2
	while(nptr != NULL)
	{
		while(comparator(ptr, nptr, 0) == TRUE)
		{
			ptr = ptr->next;
		}
		last->next = start;
		flp->last = last;
		updateStatus(flp, ptr, nptr);
		last = flp->last;
		last->next = NULL;
		nptr = nptr->next;
	}
	last->next = start;
	lptr->next = head;
	flp->last = last;
	fl2->last = lptr;
	return sc;
}

void listUnique (flight_DB *flp)
{
	// newKey is Name and ID
	// assuming the list is non-empty
	Node *last, *nptr, *ptr;
	sort(flp, 5);
	printf("\n\nSORTED LIST BEFORE REMOVING DUPLICATES : \n");
	printRec(flp, 0);
	last = flp->last;
	Node *start = last->next;
	nptr = start;
	last->next = NULL; // changing to SLL
	while(nptr->next != last)
	{
		ptr = nptr->next;
		if(ptr != NULL && isNewKeyEqual(nptr, ptr) == TRUE)
		{
			if(comparator(nptr, ptr, 4) == TRUE) // for delay_time
			{
				start = deleteNode(start, ptr);
			}
			else
			{
				start = deleteNode(start, nptr);
				nptr = ptr;
			}
		}
		else
			nptr = nptr->next;
	}
	if(nptr->next != NULL && nptr->next != nptr && isNewKeyEqual(nptr, nptr->next) == TRUE)
	{
		if(comparator(nptr, nptr->next, 4) == TRUE)
		{
			free(nptr->next);
			nptr->next = NULL;
		}
		else
		{
			start = deleteNode(start, nptr);
			nptr = ptr;
		}
	}
	ptr = start;
	while(ptr->next != NULL)
		ptr = ptr->next;
	last = ptr;
	last->next = start;
	flp->last = last;
}


int main()
{
	flight_DB my_fl, my_fl2, new_db;
	flight_DB *my_ptr = &my_fl, *my2 = &my_fl2, *newp = &new_db;
	initialize (my_ptr);
	initialize (my2);
	initialize (newp);
	// my2 for updateFlightStatus, newp for listUnique

	status_code sc=SUCCESS;
	unsigned int id=0, cap=0, key=0, del=0, hr=0, mi=0;
	time arr, dep, delay_time;
	arr.hour=0, arr.min=0, dep.hour=0, dep.min=0, delay_time.hour=0, delay_time.min=0;
	int i=0, j=0, not_failed=0, num_rec=0, select=12, flag=1, num=0;
	char name[NAME_LEN], fl_class[CLASS_LEN], fl_status[STATUS_LEN];
	Node *nptr = NULL;

	while(flag)
	{
		id=0; cap=0; key=0; del=0; hr=0; mi=0;
		arr.hour=0; arr.min=0; dep.hour=0; dep.min=0; delay_time.hour=0; delay_time.min=0;
		i=0; j=0; num_rec=0; num=0;
		name[0]='\0'; fl_class[0]='\0'; fl_status[0]='\0';

		select=12;
		printf("Please enter appropriate number each time (until you want to exit : 0) for the corresponding operation\n0. EXIT\n1. insert - to insert records\n2. delete - to delete records using id\n3. getNumFlights - get the number of flight records available\n4. isEmpty - to check if the list is empty\n5. isFull - to check if the list is fully occupied\n6. getFlightWithLongestStay - get flights with longest stay time\n7. getSortedOnArrivalTime - get the main list sorted on arrival time\n8. getSortedOnDepartureTime - get the main list sorted on departure time\n9. getSortedOnStayTime - get the main list sorted on stay time\n10. updateFlightStatus - to update the flight status\n11. listUnique\n12. to Print The Main list of records\n13. to Print The 3 Sorted Lists Of Classes\n\n");
		scanf("%d", &select);
		printf("\n");
		switch (select)
		{
			case 0: {
						flag=0;
						printf("\nSUCCESS : Exited !!\n");
					}
					break;
			case 1: {
						delay_time.hour=0;
						delay_time.min=0;
						printf("\nEnter the number of records to insert : ");
						scanf("%d",&num_rec);
						printf("\n");
						for(i=0; i<num_rec; i++)
						{
							printf("Enter the flight name ( without spaces ) : ");
							scanf("%s",name);

							printf("Enter the flight id ( non zero ) : ");
							scanf("%u",&id);

							printf("Enter the flight capacity : ");
							scanf("%u",&cap);

							printf("Enter the space separated hours(0to23) and minutes(0to59) of arrival time respectively in 24 hour format : ");
							scanf("%u%u",&arr.hour, &arr.min);

							printf("Enter the space separated hours(0to23) and minutes(0to59) of delayarture time respectively in 24 hour format : ");
							scanf("%u%u",&dep.hour, &dep.min);

							printf("Enter the flight class VIP or VVIP or public : ");
							scanf("%s",fl_class);

							if (insertUpdate(my_ptr, name, id, cap, arr, dep, delay_time, fl_class, 0) == SUCCESS)
							{
								printf("\nSUCCESS : List updated.\n\n");
								sc = insertUpdateSubList(my_ptr, name, id, cap, arr, dep, delay_time, fl_class);
							}
							else
							{
								printf("\nFAILURE : There is no space.\n\n");
							}
						}
					}
					break;
			case 2: {
						printf("\nEnter the flight id whose details are to be deleted : ");
						scanf("%u", &del);

						if (deleteList(my_ptr, del, 0) == SUCCESS)
						{
							printf("\nSUCCESS : Relevant records deleted.\n\n");
							sc = deleteSubList(my_ptr, del);
						}
						else
						{
							printf("\nFAILURE : Flight details not present.\n\n");
						}
					}
					break;
			case 3: {
						num=getNumFlights(my_ptr, 0);
						printf("\nSUCCESS: Number of flights are %d.\n\n", num);
					}
					break;
			case 4: {
						if(isEmpty(my_ptr) == TRUE)
							printf("\nSUCCESS : The list is empty.\n\n");
						else
							printf("\nSUCCESS : The list is non empty.\n\n");
					}
					break;
			case 5: {
						if(isFull(my_ptr) == TRUE)
							printf("\nSUCCESS : The list is fully occupied.\n\n");
						else
							printf("\nSUCCESS : The list is not fully occupied.\n\n");
					}
					break;
			case 6: {
						if(isEmpty(my_ptr) == TRUE)
						{
							printf("\nSUCCESS : The list is empty.\n\n");
						}
						else
						{
							printf("\nSUCCESS : Flight id and arrival time are as follows\n");
							getFlightWithLongestStay(my_ptr);
							printf("\n");
						}
					}
					break;
			case 7: {
						if(isEmpty(my_ptr) == TRUE)
						{
							printf("\nThe list is empty.\n\n");
						}
						else
						{
							getSortedOnArrivalTime(my_ptr);
							printf("\nSUCCESS : \n\n");
							printRec(my_ptr, 0);
							sort(my_ptr, 0);
						}
					}
					break;
			case 8: {
						if(isEmpty(my_ptr) == TRUE)
						{
							printf("\nThe list is empty.\n\n");
						}
						else
						{
							getSortedOnDepartureTime(my_ptr);
							printf("\nSUCCESS : \n\n");
							printRec(my_ptr, 0);
							sort(my_ptr, 0);
						}
					}
					break;
			case 9: {
						if(isEmpty(my_ptr) == TRUE)
						{
							printf("\nThe list is empty.\n\n");
						}
						else
						{
							getSortedOnStayTime(my_ptr);
							printf("\nSUCCESS : \n\n");
							printRec(my_ptr, 0);
							sort(my_ptr, 0);
						}
					}
					break;
			case 10:{
						not_failed = 1;
						initialize(my2);
						printf("\nEnter the number of records whose details are to be updated : ");
						scanf("%d",&num_rec);

						if( num_rec > DB_SIZE )
						{
							printf("\nFAILURE : Enter valid value.\n\n");
						}
						else
						{
							for (i=0; i < num_rec && not_failed; i++)
							{
								printf("\nEnter the flight name ( without spaces ) : ");
								scanf("%s",name);

								printf("Enter the flight id ( non zero ) : ");
								scanf("%u",&id);

								printf("Enter the space separated hours(0to23) and minutes(0to59) of arrival time respectively in 24 hour format : ");
								scanf("%u%u",&arr.hour, &arr.min);

								printf("Enter the flight status : ");
								scanf("%s",fl_status);

								printf("Enter the space separated hours and minutes of delay time respectively (0 0 if status is not DELAY) : ");
								scanf("%u%u",&delay_time.hour, &delay_time.min);

								nptr = makeListNode(name, id, arr, fl_status, delay_time);
								if(nptr != NULL)
								{
									my2->last = CL_InsertAtStart(nptr, my2->last);
								}
								else
								{
									printf("\nFAILURE : Heap is Full\n");
									not_failed = 0;
								}
							}
							// if not_failed == 0, only partially, updates will be done
							sc = updateFlightStatus(my_ptr, my2);
							printf("\n\nSUCCESS : \n");
							printRec(my_ptr, 0);
							printf("\n");
						}
						freeList(my2);
					}
					break;
			case 11:{
						initialize(newp);
						Node *ptr, *prev, *last, *start;
						not_failed = 1;
						printf("\nEnter the number of records (non-zero) to add : ");
						scanf("%d",&num_rec);
						prev = (Node *)malloc(sizeof(Node));
						start = prev;
						if(prev == NULL)
							printf("\nFAILURE : Heap if full.\n\n");
						else
						{
							for(i=0; i<num_rec && not_failed; i++)
							{
								ptr = (Node *)malloc(sizeof(Node));
								if(ptr == NULL)
									not_failed = 0;
								else
								{
									printf("\nEnter the flight name ( without spaces ) : ");
									scanf("%s",ptr->flight_name);

									printf("Enter the flight id ( non zero ) : ");
									scanf("%u",&ptr->flight_id);

									printf("Enter the flight capacity : ");
									scanf("%u",&ptr->flight_capacity);

									printf("Enter the space separated hours(0to23) and minutes(0to59) of arrival time respectively in 24 hour format : ");
									scanf("%u%u",&ptr->arr_time.hour, &ptr->arr_time.min);

									printf("Enter the space separated hours(0to23) and minutes(0to59) of departure time respectively in 24 hour format : ");
									scanf("%u%u",&ptr->dep_time.hour, &ptr->dep_time.min);

									printf("Enter the flight class VIP or VVIP or public : ");
									scanf("%s",ptr->flight_class);

									printf("Enter the space separated hours(0to23) and minutes(0to59) of delay time respectively (0 0 if status is not DELAY) : ");
									scanf("%u%u",&ptr->delay_time.hour, &ptr->delay_time.min);

									ptr->key = 10000*(ptr->flight_id) + 100*(ptr->arr_time.hour) + ptr->arr_time.min ;

									prev->next = ptr;
									prev = ptr;
								}

							}
						}
						if(not_failed)
						{
							ptr->next = start->next;
							last = ptr;
							newp->last = last;
							printf("\n");
							listUnique(newp);
							printf("\nSUCCESS : LIST AFTER REMOVING DUPLICATES IS\n\n");
							printRec(newp, 0);
						}
						freeList(newp);
					}
					break;
			case 12:{
						printf("\n\nLIST OF RECORDS IS : \n\n");
						printRec(my_ptr, 0);
						printf("\n");
					}
					break;
			case 13:{
						printf("\n\nVIP list\n");
						printRec(my_ptr, 1);
						printf("\n\nVVIP list\n");
						printRec(my_ptr, 2);
						printf("\n\npublic list\n");
						printRec(my_ptr, 3);
						printf("\n\n");
					}
					break;
			default:{
						flag=0;
					}
					break;
		}
	}
	freeList(my_ptr);
	freeList(my2);
	freeList(newp);
	return 0;
}
