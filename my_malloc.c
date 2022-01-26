#include "my_malloc.h"

#include <stdlib.h>
//Include shared in the Makefile

//Global variables declared here
mmalloc * head = NULL;
mmalloc * tail = NULL;
size_t allocatedBytes = 0;
size_t freeBytes = 0;
size_t freeBlocks = 0;
mmalloc * freeListHead = NULL;
mmalloc * freeListTail = NULL;

unsigned long get_data_segment_size() {
  return allocatedBytes;
}

unsigned long get_data_segment_free_space_size() {
  return freeBytes;
}

void purify(mmalloc * result) {
  if (result == freeListHead) {
    freeListHead = result->next;
    if (result->next != NULL)
      result->next->prev = NULL;
  }
  else if (result == freeListTail) {
    freeListTail = result->prev;
    if (result->prev != NULL)
      result->prev->next = NULL;
  }
  else {
    result->prev->next = result->next;
    result->next->prev = result->prev;
    result->next = NULL;
    result->prev = NULL;
  }
}

void * reusebf_malloc(size_t noBytes) {
  mmalloc * temp = freeListHead;
  size_t smallest = freeBytes + 2;
  mmalloc * result = NULL;
  size_t value = 0;
  while (temp != NULL) {
    value = temp->blockSize - noBytes - 24;
    if (value < smallest) {
      smallest = value;
      result = temp;
      /* printf("$vlaue: %lu\n", (result)->blockSize); */
    }
    temp = temp->next;
  }
  if (smallest == freeBytes + 2) {
    return NULL;
  }
  purify(result);
  return result;
}

void addToMalloc(mmalloc * newNode) {
  newNode->next = NULL;
  newNode->prev = tail;
  tail = newNode;
  if (head == NULL) {
    head = tail;
  }
  else {
    tail->prev->next = tail;
  }
}

void * bf_malloc(size_t noBytes) {
  void * result = reusebf_malloc(noBytes);
  if (result == NULL) {
    mmalloc * newNode = sbrk(noBytes + sizeof(mmalloc));
    newNode->blockSize = noBytes + sizeof(mmalloc);
    addToMalloc(newNode);
    allocatedBytes += tail->blockSize;
    void * anotherOne = tail;
    /* printf("result is: %p", anotherOne + sizeof(mmalloc) + 1); */
    return anotherOne + sizeof(mmalloc);
    /* return anotherOne; */
  }
  mmalloc * resPtr = result;
  freeBytes -= resPtr->blockSize;
  /* printf("result is: %p", result + sizeof(mmalloc) + 1); */
  addToMalloc(result);
  return result + sizeof(mmalloc);
  /* return result; */
}

void addSortedFree(mmalloc * ptrToAdd) {
  allocatedBytes = allocatedBytes - ptrToAdd->blockSize;
  freeBytes = freeBytes + ptrToAdd->blockSize;
  size_t value = mergeFreeList(ptrToAdd);
  if (value == 0) {
    ptrToAdd->next = NULL;
    ptrToAdd->prev = freeListTail;
    freeListTail = ptrToAdd;
    if (freeListHead == NULL) {
      freeListHead = freeListTail;
    }
    else {
      freeListTail->prev->next = freeListTail;
    }
  }
  /* mmalloc * temp = freeListHead; */
  /* while (temp->next != NULL && temp->next < ptrToAdd) { */
  /*   temp = temp->next; */
  /* } */
  /* ptrToAdd->next = temp->next; */
  /* if (temp->next != NULL) { */
  /*   ptrToAdd->next->prev = ptrToAdd; */
  /* } */

  /* temp->next = ptrToAdd; */
  /* ptrToAdd->prev = temp; */
  /* while (temp->next != NULL) { */
  /*   temp = temp->next; */
  /* } */
  /* freeListTail = temp; */

  /* if (freeListHead == NULL) { */
  /*   freeListHead = ptrToAdd; */
  /*   freeListTail = freeListHead; */
  /*   freeListHead->prev = NULL; */
  /*   return; */
  /* } */
  /* if (ptrToAdd < freeListHead->next) { */
  /*   ptrToAdd->prev = NULL; */
  /*   freeListHead->prev = ptrToAdd; */
  /*   ptrToAdd->next = freeListHead; */
  /*   freeListHead = ptrToAdd; */
  /*   return; */
  /* } */
  /* if (ptrToAdd > freeListTail) { */
  /*   ptrToAdd->prev = freeListTail; */
  /*   freeListTail->next = ptrToAdd; */
  /*   freeListTail = ptrToAdd; */
  /*   return; */
  /* } */
  /* ptrToAdd->next = freeListHead; */
  /* ptrToAdd->prev = NULL; */
  /* freeListHead = ptrToAdd; */
}

void bf_free(void * ptrToDeleteVoid) {
  //This function will work considering the bf_malloc function makes
  //a properly functioning linkedlist
  /* mmalloc * ptrToDelete = (ptrToDeleteVoid - sizeof(mmalloc) - 1); */
  ptrToDeleteVoid -= sizeof(mmalloc);
  mmalloc * ptrToDelete = ptrToDeleteVoid;
  if (ptrToDelete->next == NULL && ptrToDelete->prev == NULL) {
    head = NULL;
    tail = NULL;
  }
  else if (ptrToDelete->next == NULL) {
    tail = ptrToDelete->prev;
    ptrToDelete->prev->next = NULL;
    ptrToDelete->prev = NULL;
  }
  else if (ptrToDelete->prev == NULL) {
    head = ptrToDelete->next;
    ptrToDelete->next->prev = NULL;
    ptrToDelete->next = NULL;
  }
  else {
    ptrToDelete->next->prev = ptrToDelete->prev;
    ptrToDelete->prev->next = ptrToDelete->next;
    ptrToDelete->next = NULL;
    ptrToDelete->prev = NULL;
  }
  ++freeBlocks;
  addSortedFree(ptrToDelete);
}

void printLLFront(mmalloc * headPtr) {
  mmalloc * temp = headPtr;
  printf("******************In printLL Front******************\n");
  size_t i = 0;
  while (temp != NULL) {
    printf("\t\t\t%p\n", temp);
    printf("Data allocated here is: %lu\n", temp->blockSize);
    if (temp->prev == NULL) {
      printf("Yep its null\n");
    }
    else {
      printf("Its not null: %lu\n", temp->prev->blockSize);
    }
    temp = temp->next;
    ++i;
  }
  headPtr = temp;
}

void printLLBack(mmalloc * tailPtr) {
  mmalloc * temp = tailPtr;
  printf("******************In printLL Back******************\n");
  size_t i = 0;
  while (tailPtr != NULL) {
    printf("Data allocated from the back here is: %lu\n", tailPtr->blockSize);
    tailPtr = tailPtr->prev;
    ++i;
  }
  tailPtr = temp;
}

size_t mergeFreeList(mmalloc * newNode) {
  mmalloc * temp = freeListHead;
  void * tempVoid = temp;
  void * newNodeVoid = newNode;
  while (temp != NULL) {
    size_t value = 0;
    if (tempVoid > newNodeVoid) {
      value = tempVoid - newNodeVoid;
      /* printf("The value is %lu\n", value); */
    }
    else {
      value = newNodeVoid - tempVoid;
      /* printf("The value is %lu in the else block\n", value); */
    }
    if (value == temp->blockSize) {
      temp->blockSize += newNode->blockSize;
      return 1;
    }
    temp = temp->next;
  }
  return 0;

  /* mmalloc * temp = freeListHead; */
  /* while (temp != NULL && temp->next != NULL) { */
  /*   void * ptr1 = temp; */
  /*   void * ptr2 = temp->next; */
  /*   if ((ptr2 - ptr1) == temp->blockSize) { */
  /*     --freeBlocks; */
  /*     temp->blockSize += temp->next->blockSize; */
  /*     mmalloc ** newPtr = &((temp->next)->next); */
  /*     temp->next = *newPtr; */
  /*     if (*newPtr != NULL) { */
  /*       (*newPtr)->prev = NULL; */
  /*       (*newPtr) = NULL; */
  /*     } */
  /*   } */
  /*   temp = temp->next; */
  /* } */
  /* if (freeListHead->blockSize == freeBytes) { */
  /*   freeListTail = freeListHead; */
  /* } */
}

void * ff_malloc(size_t noBytes) {
  if (freeListHead != NULL && (noBytes < ((freeListHead->blockSize) - sizeof(mmalloc)))) {
    mmalloc * result = NULL;
    result = freeListHead;

    /* purify(result); */
    void * resPtr = result;
    freeListHead = freeListHead->next;
    return resPtr + sizeof(mmalloc);
  }
  else {
    mmalloc * newNode = sbrk(noBytes + sizeof(mmalloc));
    newNode->blockSize = noBytes + sizeof(mmalloc);
    addToMalloc(newNode);
    allocatedBytes += tail->blockSize;
    void * anotherOne = tail;
    /* printf("result is: %p", anotherOne + sizeof(mmalloc) + 1); */
    return anotherOne + sizeof(mmalloc);
  }
}

void ff_free(void * ptrToDeleteVoid) {
  /* ptrToDeleteVoid -= sizeof(mmalloc); */
  ptrToDeleteVoid -= sizeof(mmalloc);
  mmalloc * ptrToDelete = ptrToDeleteVoid;
  if (ptrToDelete->next == NULL && ptrToDelete->prev == NULL) {
    head = NULL;
    tail = NULL;
  }
  else if (ptrToDelete->next == NULL) {
    tail = ptrToDelete->prev;
    ptrToDelete->prev->next = NULL;
    ptrToDelete->prev = NULL;
  }
  else if (ptrToDelete->prev == NULL) {
    head = ptrToDelete->next;
    ptrToDelete->next->prev = NULL;
    ptrToDelete->next = NULL;
  }
  else {
    ptrToDelete->next->prev = ptrToDelete->prev;
    ptrToDelete->prev->next = ptrToDelete->next;
    ptrToDelete->next = NULL;
    ptrToDelete->prev = NULL;
  }
  ++freeBlocks;
  addSortedFree(ptrToDelete);
}

/* int main() { */
/*   void * ptr1 = bf_malloc(100); */
/*   void * ptr2 = bf_malloc(200); */
/*   void * ptr3 = bf_malloc(300); */
/*   void * ptr4 = bf_malloc(400); */
/*   void * ptr5 = bf_malloc(500); */
/*   bf_free(ptr1); */
/*   bf_free(ptr3); */
/*   bf_free(ptr5); */
/*   void * ptr6 = bf_malloc(20); */
/*   printLLFront(head); */
/*   void * ptr7 = bf_malloc(40); */
/*   printLLFront(head); */
/*   printLLFront(freeListHead); */
/*   return 0; */
/* } */

/* printf("\t\t\tAllocated Bytes: %lu, Free Bytes: %lu\n", allocatedBytes, freeBytes); */

/* size_t canMerge(mmalloc * ptr1, mmalloc * ptr2) { */
/*   void * newPtr = ptr1; */
/*   void * anotherPtr = ptr2; */
/*   size_t value = 0; */
/*   if (anotherPtr > newPtr) { */
/*     value = anotherPtr - newPtr; */
/*   } */
/*   else { */
/*     value = newPtr - anotherPtr; */
/*   } */
/*   return (value == ptr1->blockSize) ? 1 : 0; */
/*   /\* mmalloc * veryNewPtr = ptr2 + sizeof(mmalloc *) + 1; *\/ */
/*   /\* veryNewPtr->blockSize = ptr2->blockSize - (ptr2->blockSize - (sizeof(mmalloc *) + 1)); *\/ */
/*   /\* void * ptr3 = veryNewPtr; *\/ */
/*   /\* if (ptr3 > anotherPtr) { *\/ */
/*   /\*   printf("HELL YES"); *\/ */
/*   /\* } *\/ */
/* } */
