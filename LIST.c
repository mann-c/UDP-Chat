#include<stddef.h>
#include<stdio.h>
#include<stdbool.h>
#include "LIST.h"

// determine if this is the first time ListCreate is being called
int firstListCreateCall = 1;

// variables for the stack of the empty indices in the listArr, initialized with random values
int listStackHead = beyondStart;
int listStackSize = 0;

// variables for the stack of the empty indices in the nodeArr, initialized with random values
int nodeStackHead = beyondStart;
int nodeStackSize = 0;

void pushEmptyListArr(int index) {
  listStackSize++;
  listArr[index].nextList = listStackHead;
  listStackHead = index;
  return;
}

int popEmptyListArr() {
  if(listStackSize > 0) {
    int tempIndex = listStackHead;
    listStackSize--;
    listStackHead = listArr[listStackHead].nextList;
    return tempIndex;
  }
  return beyondEnd;
}

void pushEmptyNodeArr(int index) {
  nodeStackSize++;
  nodeArr[index].nextNode = nodeStackHead;
  nodeStackHead = index;
  return;
}

int popEmptyNodeArr() {
  if(nodeStackSize > 0) {
    int tempIndex = nodeStackHead;
    nodeStackSize--;
    nodeStackHead = nodeArr[nodeStackHead].nextNode;
    return tempIndex;
  }
  return beyondEnd;
}

LIST* ListCreate() {
  // intialize the empty node and list stacks, and create the first list
  if(firstListCreateCall == 1) {

    // traverse the listArr and add all of the indices in the listArr into the empty list array stack
    for(int i = numberOfLists-1; i >= 0; i--) {
      listArr[i].currentIndex = i;
      pushEmptyListArr(i);
    }
    // traverse the nodeArr and add all of the indices in the nodeArr into the empty node array stack
    for(int i = numberOfNodes-1; i >= 0; i--) {
      nodeArr[i].currentIndex = i;
      pushEmptyNodeArr(i);
    }
    firstListCreateCall = 0;
  }

  // check if there are any empty list array indices left
  if(listStackSize > 0) {
    int tempIndex = popEmptyListArr();
    listArr[tempIndex].listHead = (int)NULL;
    listArr[tempIndex].listTail = (int)NULL;
    listArr[tempIndex].sizeOfList = 0;
    listArr[tempIndex].currentNode = (int)NULL;
    return &listArr[tempIndex];
  }
  return NULL;
}

int ListCount(LIST* list) {
  return list->sizeOfList;
}

void* ListFirst(LIST* list) {
  int listIndex = list->currentIndex;

  // determine if the list has any nodes
  if(listArr[listIndex].sizeOfList > 0) {
    listArr[listIndex].currentNode = listArr[listIndex].listHead;
    return nodeArr[listArr[listIndex].currentNode].value;
  }
  return NULL;
}

void* ListLast(LIST* list) {
  int listIndex = list->currentIndex;

  // determine if the list has any nodes
  if(listArr[listIndex].sizeOfList > 0) {
    listArr[listIndex].currentNode = listArr[listIndex].listTail;
    return nodeArr[listArr[listIndex].currentNode].value;
  }
  return NULL;
}

void* ListNext(LIST* list) {
  int listIndex = list->currentIndex;

  // determine if the list has any nodes and whether its current pointer is beyond the end of the list
  if(listArr[listIndex].sizeOfList > 0 && listArr[listIndex].currentNode != beyondEnd) {
    listArr[listIndex].currentNode = nodeArr[listArr[listIndex].currentNode].nextNode;

    // determine if the current pointer has been moved to beyond the end of the list
    if(listArr[listIndex].currentNode == beyondEnd) {
      return NULL;
    }
    else {
      return nodeArr[listArr[listIndex].currentNode].value;
    }
  }
  return NULL;
}

void* ListPrev(LIST* list) {
  int listIndex = list->currentIndex;

  // determine if the list has any nodes and whether its current pointer is beyond the end of the list
  if(listArr[listIndex].sizeOfList > 0 && listArr[listIndex].currentNode != beyondStart) {
    listArr[listIndex].currentNode = nodeArr[listArr[listIndex].currentNode].prevNode;

    // determine if the current pointer has been moved to beyond the end of the list
    if(listArr[listIndex].currentNode == beyondStart) {
      return NULL;
    }
    else {
      return nodeArr[listArr[listIndex].currentNode].value;
    }
  }
  return NULL;
}

void* ListCurr(LIST* list) {
  return nodeArr[list->currentNode].value;
}

int ListAdd(LIST* list, void* item) {
  // determine if there are any empty nodes available to expand the list
  int indexNewNode = popEmptyNodeArr();
  if(indexNewNode == beyondEnd)
    return -1;

  int listIndex = list->currentIndex;
  // if the list is empty then add the node in the first position, which would also be the last position
  if(listArr[listIndex].sizeOfList == 0) {
    nodeArr[indexNewNode].value = item;
    nodeArr[indexNewNode].nextNode = beyondEnd;
    nodeArr[indexNewNode].prevNode = beyondStart;
    listArr[listIndex].listHead = indexNewNode;
    listArr[listIndex].listTail = indexNewNode;
    listArr[listIndex].sizeOfList++;
    listArr[listIndex].currentNode = indexNewNode;
    return 0;
  }
  // if the list isn't empty and the current node is before the start of the list,
  // add the node at the start of the list
  else if(listArr[listIndex].sizeOfList > 0 && listArr[listIndex].currentNode == beyondStart) {
    nodeArr[indexNewNode].value = item;
    nodeArr[indexNewNode].nextNode = listArr[listIndex].listHead;
    nodeArr[indexNewNode].prevNode = beyondStart;
    nodeArr[listArr[listIndex].listHead].prevNode = indexNewNode;
    listArr[listIndex].listHead = indexNewNode;
    listArr[listIndex].sizeOfList++;
    listArr[listIndex].currentNode = indexNewNode;
    return 0;
  }
  // if the list isn't empty and the current node is beyond the end of the list,
  // add the node at the end of the list
  else if(listArr[listIndex].sizeOfList > 0 && listArr[listIndex].currentNode == beyondEnd) {
    nodeArr[indexNewNode].value = item;
    nodeArr[indexNewNode].nextNode = beyondEnd;
    nodeArr[indexNewNode].prevNode = listArr[listIndex].listTail;
    nodeArr[listArr[listIndex].listTail].nextNode = indexNewNode;
    listArr[listIndex].listTail = indexNewNode;
    listArr[listIndex].currentNode = indexNewNode;
    listArr[listIndex].sizeOfList++;
    return 0;
  }
  // if the list isn't empty and current node is not beyond the end or start of the list,
  // add the new node into list after the current node
  else if(listArr[listIndex].sizeOfList > 0) {
    nodeArr[indexNewNode].value = item;
    nodeArr[indexNewNode].nextNode = nodeArr[listArr[listIndex].currentNode].nextNode;
    nodeArr[indexNewNode].prevNode = listArr[listIndex].currentNode;
    if(nodeArr[listArr[listIndex].currentNode].nextNode != beyondEnd)
     nodeArr[nodeArr[listArr[listIndex].currentNode].nextNode].prevNode = indexNewNode;
    nodeArr[listArr[listIndex].currentNode].nextNode = indexNewNode;
    listArr[listIndex].sizeOfList++;
    listArr[listIndex].currentNode = indexNewNode;
    return 0;
  }
  return -1;
}

int ListInsert(LIST* list, void* item) {
  // determine if there are any empty nodes available to expand the list
  int indexNewNode = popEmptyNodeArr();
  if(indexNewNode == beyondEnd)
    return -1;

  int listIndex = list->currentIndex;
  // if the list is empty then add the node in the first position, which would also be the last position
  if(listArr[listIndex].sizeOfList == 0) {
    nodeArr[indexNewNode].value = item;
    nodeArr[indexNewNode].nextNode = beyondEnd;
    nodeArr[indexNewNode].prevNode = beyondStart;
    listArr[listIndex].listHead = indexNewNode;
    listArr[listIndex].listTail = indexNewNode;
    listArr[listIndex].sizeOfList++;
    listArr[listIndex].currentNode = indexNewNode;
    return 0;
  }
  // if the list isn't empty and the current node is before the start of the list,
  // add the node at the start of the list
  else if(listArr[listIndex].sizeOfList > 0 && listArr[listIndex].currentNode == beyondStart) {
    nodeArr[indexNewNode].value = item;
    nodeArr[indexNewNode].nextNode = listArr[listIndex].listHead;
    nodeArr[indexNewNode].prevNode = beyondStart;
    nodeArr[listArr[listIndex].listHead].prevNode = indexNewNode;
    listArr[listIndex].listHead = indexNewNode;
    listArr[listIndex].sizeOfList++;
    listArr[listIndex].currentNode = indexNewNode;
    return 0;
  }
  // if the list isn't empty and the current node is beyond the end of the list,
  // add the node at the end of the list
  else if(listArr[listIndex].sizeOfList > 0 && listArr[listIndex].currentNode == beyondEnd) {
    nodeArr[indexNewNode].value = item;
    nodeArr[indexNewNode].nextNode = beyondEnd;
    nodeArr[indexNewNode].prevNode = listArr[listIndex].listTail;
    nodeArr[listArr[listIndex].listTail].nextNode = indexNewNode;
    listArr[listIndex].listTail = indexNewNode;
    listArr[listIndex].currentNode = indexNewNode;
    listArr[listIndex].sizeOfList++;
    return 0;
  }
  // if the list isn't empty and current node is not beyond the end or start of the list,
  // add the new node into list before the current node
  else if(listArr[listIndex].sizeOfList > 0) {
    nodeArr[indexNewNode].value = item;
    nodeArr[indexNewNode].nextNode = listArr[listIndex].currentNode;
    nodeArr[indexNewNode].prevNode = nodeArr[listArr[listIndex].currentNode].prevNode;
    nodeArr[nodeArr[listArr[listIndex].currentNode].prevNode].nextNode = indexNewNode;
    nodeArr[listArr[listIndex].currentNode].prevNode = indexNewNode;
    listArr[listIndex].sizeOfList++;
    listArr[listIndex].currentNode = indexNewNode;
    return 0;
  }
  return -1;
}

int ListAppend(LIST* list, void* item) {
  // determine if there are any empty nodes available to expand the list
  int indexNewNode = popEmptyNodeArr();
  if(indexNewNode == beyondEnd)
    return -1;

  int listIndex = list->currentIndex;

  // determine if the list is empty
  if(listArr[listIndex].sizeOfList == 0) {
    nodeArr[indexNewNode].value = item;
    nodeArr[indexNewNode].nextNode = beyondEnd;
    nodeArr[indexNewNode].prevNode = beyondStart;
    listArr[listIndex].listHead = indexNewNode;
    listArr[listIndex].listTail = indexNewNode;
    listArr[listIndex].sizeOfList++;
    listArr[listIndex].currentNode = indexNewNode;
    return 0;
  }
  else {
    nodeArr[indexNewNode].value = item;
    nodeArr[listArr[listIndex].listTail].nextNode = indexNewNode;
    nodeArr[indexNewNode].prevNode = listArr[listIndex].listTail;
    nodeArr[indexNewNode].nextNode = beyondEnd;
    listArr[listIndex].listTail = indexNewNode;
    listArr[listIndex].sizeOfList++;
    listArr[listIndex].currentNode = indexNewNode;
    return 0;
  }
}

int ListPrepend(LIST* list, void* item) {
  // determine if there are any empty nodes available to expand the list
  int indexNewNode = popEmptyNodeArr();
  if(indexNewNode == beyondEnd)
    return -1;

  int listIndex = list->currentIndex;

  // determine if the list is empty
  if(listArr[listIndex].sizeOfList == 0) {
    nodeArr[indexNewNode].value = item;
    nodeArr[indexNewNode].nextNode = beyondEnd;
    nodeArr[indexNewNode].prevNode = beyondStart;
    listArr[listIndex].listHead = indexNewNode;
    listArr[listIndex].listTail = indexNewNode;
    listArr[listIndex].sizeOfList++;
    listArr[listIndex].currentNode = indexNewNode;
    return 0;
  }
  else {
    nodeArr[indexNewNode].value = item;
    nodeArr[listArr[listIndex].listHead].prevNode = indexNewNode;
    nodeArr[indexNewNode].nextNode = listArr[listIndex].listHead;
    nodeArr[indexNewNode].prevNode = beyondStart;
    listArr[listIndex].listHead = indexNewNode;
    listArr[listIndex].sizeOfList++;
    listArr[listIndex].currentNode = indexNewNode;
    return 0;
  }
}

void* ListRemove(LIST* list) {
  int listIndex = list->currentIndex;
  // determine if the list is empty or if the current pointer is beyond the start or end of the list,
  // then return null
  if(listArr[listIndex].sizeOfList == 0 || listArr[listIndex].currentNode == beyondEnd || listArr[listIndex].currentNode == beyondStart) {
    return NULL;
  }
  // if the list is of size 1
  if(listArr[listIndex].sizeOfList == 1) {
    void* tempItem = nodeArr[listArr[listIndex].listHead].value;
    nodeArr[listArr[listIndex].listHead].value = NULL;
    pushEmptyNodeArr(listArr[listIndex].listHead);
    listArr[listIndex].sizeOfList--;
    listArr[listIndex].listHead = (int)NULL;
    listArr[listIndex].listTail = (int)NULL;
    listArr[listIndex].currentNode = (int)NULL;
    return tempItem;
  }
  // if the current node is at the list head then we need an else if statement to handle the scenario
  else if(listArr[listIndex].currentNode == listArr[listIndex].listHead) {
    int tempNextNode = nodeArr[listArr[listIndex].currentNode].nextNode;
    void* tempItem = nodeArr[listArr[listIndex].currentNode].value;
    nodeArr[listArr[listIndex].currentNode].value = NULL;
    listArr[listIndex].sizeOfList--;
    pushEmptyNodeArr(listArr[listIndex].currentNode);
    listArr[listIndex].currentNode = tempNextNode;
    listArr[listIndex].listHead = tempNextNode;
    nodeArr[listArr[listIndex].listHead].prevNode = beyondStart;
    return tempItem;
  }
  // if the current node is at the list tail then we need an else if statement to handle the scenario
  else if(listArr[listIndex].currentNode == listArr[listIndex].listTail) {
    int tempPrevNode = nodeArr[listArr[listIndex].currentNode].prevNode;
    void* tempItem = nodeArr[listArr[listIndex].currentNode].value;
    nodeArr[listArr[listIndex].currentNode].value = NULL;
    listArr[listIndex].sizeOfList--;
    pushEmptyNodeArr(listArr[listIndex].currentNode);
    listArr[listIndex].listTail = tempPrevNode;
    listArr[listIndex].currentNode = beyondEnd;
    nodeArr[listArr[listIndex].currentNode].nextNode = beyondEnd;
    return tempItem;
  }
  else {
    // connect the doubly linked list with the two nodes surrounding the current node
    nodeArr[nodeArr[listArr[listIndex].currentNode].prevNode].nextNode = nodeArr[listArr[listIndex].currentNode].nextNode;
    nodeArr[nodeArr[listArr[listIndex].currentNode].nextNode].prevNode = nodeArr[listArr[listIndex].currentNode].prevNode;
    listArr[listIndex].sizeOfList--;
    // save the next node to make it the current index when the current node has been pushed onto the empty stack
    int tempNextNode = nodeArr[listArr[listIndex].currentNode].nextNode;
    void* tempItem = nodeArr[listArr[listIndex].currentNode].value;
    nodeArr[listArr[listIndex].currentNode].value = NULL;
    // push the empty node onto the empty node stack
    pushEmptyNodeArr(listArr[listIndex].currentNode);
    listArr[listIndex].currentNode = tempNextNode;
    return tempItem;
  }
}

void ListConcat(LIST* list1, LIST* list2) {
  int list1Index = list1->currentIndex;
  int list2Index = list2->currentIndex;

  if(listArr[list1Index].sizeOfList == 0 && listArr[list2Index].sizeOfList == 0) {
    pushEmptyListArr(list2Index);
  }
  else if(listArr[list1Index].sizeOfList == 0) {
    listArr[list1Index].listHead = listArr[list2Index].listHead;
    listArr[list1Index].listTail = listArr[list2Index].listTail;
    listArr[list1Index].sizeOfList = listArr[list2Index].sizeOfList;
    pushEmptyListArr(list2Index);
  }
  else if(listArr[list2Index].sizeOfList == 0) {
    pushEmptyListArr(list2Index);
  }
  else {
    nodeArr[listArr[list1Index].listTail].nextNode = listArr[list2Index].listHead;
    nodeArr[listArr[list2Index].listHead].prevNode = listArr[list1Index].listTail;
    listArr[list1Index].listTail = listArr[list2Index].listTail;
    listArr[list1Index].sizeOfList += listArr[list2Index].sizeOfList;
    pushEmptyListArr(list2Index);
  }
}

void ListFree(LIST* list, void (*itemFree)(void*)) {
  int listIndex = list->currentIndex;

  // determine if the list is of size 0, then just push the list head into empty list stack
  if(listArr[listIndex].sizeOfList == 0) {
    pushEmptyListArr(listIndex);
    return;
  }
  // else move through the list, free each item and put the node into the empty node stack
  else {
    int deleteNode;
    for(int i = 0; i < listArr[listIndex].sizeOfList; i++) {
      deleteNode = listArr[listIndex].listHead;
      (*itemFree)(nodeArr[deleteNode].value);
      listArr[listIndex].listHead = nodeArr[deleteNode].nextNode;
      pushEmptyNodeArr(deleteNode);
    }
    pushEmptyListArr(listIndex);
    return;
  }
}

void* ListTrim(LIST* list) {
  int listIndex = list->currentIndex;

  if(listArr[listIndex].sizeOfList == 0) {
    return NULL;
  }
  else if(listArr[listIndex].sizeOfList == 1) {
    void* item = nodeArr[listArr[listIndex].listHead].value;
    nodeArr[listArr[listIndex].listHead].value =NULL;
    pushEmptyNodeArr(listArr[listIndex].listHead);
    listArr[listIndex].sizeOfList--;
    listArr[listIndex].listHead = (int)NULL;
    listArr[listIndex].listTail = (int)NULL;
    listArr[listIndex].currentNode = (int)NULL;
    return item;
  }
  else {
    void* item = nodeArr[listArr[listIndex].listTail].value;
    int tempPrevNode = nodeArr[listArr[listIndex].listTail].prevNode;
    listArr[listIndex].sizeOfList--;
    nodeArr[listArr[listIndex].listTail].value = NULL;
    pushEmptyNodeArr(listArr[listIndex].listTail);
    listArr[listIndex].listTail = tempPrevNode;
    listArr[listIndex].currentNode = tempPrevNode;
    nodeArr[listArr[listIndex].currentNode].nextNode = beyondEnd;
    return item;
  }
}

void* ListSearch(LIST* list, int (*comparator)(void*, void*), void* comparisonArg) {
  int listIndex = list->currentIndex;

  // determine if the list is of size 0, then just return a null
  if(listArr[listIndex].sizeOfList == 0) {
    listArr[listIndex].currentNode = beyondEnd;
    return NULL;
  }
  // else move through the list and try to find the match to the item
  else {
    int checkNode = listArr[listIndex].currentNode;
    while(checkNode != beyondEnd) {
      if((*comparator)(nodeArr[checkNode].value, comparisonArg)) {
        listArr[listIndex].currentNode = checkNode;
        return nodeArr[checkNode].value;
      }
      checkNode = nodeArr[checkNode].nextNode;
    }
    listArr[listIndex].currentNode = beyondEnd;
    return NULL;
  }
}

// debugging, print out the entire node array
void printNodeArray(int howManyNodesPrinted) {
  for(int i = 0; i < howManyNodesPrinted; i++) {
    if(nodeArr[i].value == NULL)
      printf("Node Index: %d, prev node: %d, next node: %d, node value: NULL\n", nodeArr[i].currentIndex, nodeArr[i].prevNode, nodeArr[i].nextNode);
    else
      printf("Node Index: %d, prev node: %d, next node: %d, node value: %d\n", nodeArr[i].currentIndex, nodeArr[i].prevNode, nodeArr[i].nextNode, *((int*)nodeArr[i].value));
  }
}

// required to check if there are any more nodes available
bool isListFull() {
	if(nodeStackSize <= 0)
		return true;
	else
		return false;
}
