#include<stdbool.h>
#define numberOfLists 10
#define numberOfNodes 50
#define beyondStart -2
#define beyondEnd -1

typedef struct LIST {
  int currentIndex;
  int nextList;
  int listHead;
  int listTail;
  int sizeOfList;
  int currentNode;
} LIST;

// struct definition for the nodes
typedef struct node {
  void* value;
  int currentIndex;
  int nextNode;
  int prevNode;
} node;

// create two statically allocated arrays for the list heads and nodes
LIST listArr[10];
node nodeArr[50];

// pop function for empty listArr stack, returns the index of an available spot in the listArr
int popEmptyListArr();

// push function for empty listArr stack
void pushEmptyListArr(int index);

// pop function for empty node stack, returns the index of an available spot in the nodeArr
int popEmptyNodeArr();

// push function for empty node stack
void pushEmptyNodeArr();

// ListCreate function, creates a list, stores it in the listArr, returns pointer to the list
LIST* ListCreate();

int ListCount(LIST* list);

void* ListFirst(LIST* list);

void* ListLast(LIST* list);

void* ListNext(LIST* list);

void* ListPrev(LIST* list);

void* ListCurr(LIST* list);

int ListAdd(LIST* list, void* item);

int ListInsert(LIST* list, void* item);

int ListAppend(LIST* list, void* item);

int ListPrepend(LIST* list, void* item);

void* ListRemove(LIST* list);

void ListConcat(LIST* list1, LIST* list2);

void ListFree(LIST* list, void (*itemFree)(void*));

void* ListTrim(LIST* list);

void* ListSearch(LIST* list, int (*comparator)(void*, void*), void* comparisonArg);

void printNodeArray();

bool isListFull();
