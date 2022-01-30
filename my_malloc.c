#include "my_malloc.h"

#include <stdlib.h>

size_t allocatedBytes = 0;
size_t freeBytes = 0;

mmalloc * fHead = NULL;
mmalloc * fTail = NULL;

unsigned long get_data_segment_size() {
  return allocatedBytes;
}

unsigned long get_data_segment_free_space_size() {
  return freeBytes;
}

/*
This function determines whether a block from the freeList can be used or not.
It assumes that the pointer in the free linked list is one with metadata attached to it.
If it can then it returns the original pointer with the metadata information
the metadata size is to be calculate in the bf_malloc function.
 */
void * reusebf_malloc(size_t noBytes) {
  if (fHead == NULL) {
    printf("This is a corner case. It means that noBytes < freeBytes was evaluated to "
           "true and fHead is empty. Please check add to free list function(guess)\n");
    return NULL;
  }
  size_t valMmalloc = sizeof(mmalloc);
  mmalloc * temp = fHead;
  size_t smallest = 0;
  mmalloc * result = NULL;
  size_t value = 0;
  while (temp != NULL) {
    if ((temp->blockSize - valMmalloc) >= noBytes) {
      value = temp->blockSize;
      if (value < smallest) {
        smallest = value;
        result = temp;
      }
    }
    temp = temp->next;
  }
  return result;
}

/*
  This method basically clears ensures proper removal of this pointer 
  and splits the memory block if necessary
*/
void * splitBlocks(mmalloc * ptrToPurify, size_t noBytes) {
  void * pTPVoid = ptrToPurify;
  if (ptrToPurify->blockSize + (2 * sizeof(mmalloc)) > noBytes) {
    void * tempVoid = pTPVoid + sizeof(mmalloc) + 1;
    printf("************%p, %lu:\n", tempVoid, ptrToPurify->blockSize);
  }
  return NULL;
}

/*
This function returns either a new block if the noBytes requested
is less than the freeBlocks
Else it creates a new block by sbrk system call
The data address after incrementing it with sizeof(mmalloc) is returned
*/
void * bf_malloc(size_t noBytes) {
  /* if (noBytes < freeBytes) { */
  /*   void * result = reusebf_malloc(noBytes); */
  /*   result = splitBlocks(result, noBytes); */
  /* } */
  mmalloc * newNode = sbrk(noBytes + sizeof(mmalloc));
  newNode->blockSize = noBytes + sizeof(mmalloc);
  allocatedBytes += noBytes;
  void * newNodeVoid = newNode;
  return (newNodeVoid + sizeof(mmalloc));
}

/*
  This function is supposed to merge the pointer that is to
  be added to the linked list with any adjacent pointers that 
  are already present in the free linked list.
  This function assumes that the parameter contains the metadata
  in the blocksize
*/

size_t tryMerge(mmalloc * ptrToMerge) {
  printf("Coming in tryMerge\n");
  mmalloc * temp = fHead;
  void * tempVoid;
  void * ptrVoid = ptrToMerge;
  void * smallerVoid;
  void * biggerVoid;
  mmalloc * smallerM;
  mmalloc * biggerM;
  size_t result = 0;
  size_t value = 0;
  while (temp != NULL) {
    tempVoid = temp;
    if (tempVoid > ptrVoid) {
      smallerVoid = ptrVoid;
      biggerVoid = tempVoid;
    }
    else if (tempVoid == ptrVoid) {
      printf("This is an error case. This statement should never be printed. But if it "
             "is then there is something wrong with the pointer address manipulation.\n");
    }
    else {
      smallerVoid = tempVoid;
      biggerVoid = ptrVoid;
    }
    smallerM = (mmalloc *)smallerVoid;
    biggerM = (mmalloc *)biggerVoid;
    value = biggerVoid - smallerVoid;
    printf("Bigger is: %p, smaller is: %p\n", biggerVoid, smallerVoid);
    if (value == smallerM->blockSize) {
      /*
        Case in which smallerM is in the linked list.
        Since smallerM is supposed to "absorb" biggerM,
        we don't have to manipulate biggerM's pointers, 
        since they are NULL.
        We just increase smallerM's blockSize
      */
      if (smallerM != ptrToMerge) {
        smallerM->blockSize = smallerM->blockSize + biggerM->blockSize;
      }
      /*
        Case in which smallerM is not in the linked list.
        It is actually the ptr that is supposed to be merged.
        Hence, we just assign bigger's pointers to smaller and
        then replace bigger's place in the linked list with smaller.
        Set it to temp and we increment its blockSize.
      */
      else {
        smallerM->prev = biggerM->prev;
        smallerM->next = biggerM->next;
        if (biggerM->prev != NULL) {
          biggerM->prev->next = smallerM;
        }
        else {
          fHead = smallerM;
        }
        if (biggerM->next != NULL) {
          biggerM->next->prev = smallerM;
        }
        else {
          fTail = smallerM;
        }
        biggerM->next = NULL;
        biggerM->prev = NULL;
        temp = smallerM;
        smallerM->blockSize += biggerM->blockSize;
      }
      result++;
    }
    temp = temp->next;
  }
  return result;
}

/*
  This function is responsible for adding the paramter to the free 
  linked list.
  It assumes that the parameter's address starts after the area 
  occupied in this block by the metadata
*/

void bf_free(void * ptrToFree) {
  ptrToFree -= sizeof(mmalloc);
  mmalloc * ptr = ptrToFree;
  size_t merged;
  merged = tryMerge(ptr);
  /* printf("The value of merged after trying to merge %p is: %lu\n", ptrToFree, merged); */
  if (merged == 0) {
    ptr->next = NULL;
    ptr->prev = fTail;
    fTail = ptr;
    if (fHead == NULL) {
      fHead = fTail;
    }
    else {
      fTail->prev->next = fTail;
    }
  }
}

/*
  This function was purely made for testing purposes.
  It will print the blocksizes and addresses of the 
  free linked list starting from the head pointer.
*/

void printLLFront() {
  mmalloc * temp = fHead;
  while (temp != NULL) {
    printf("**********%p, %lu**********\n", temp, temp->blockSize);
    temp = temp->next;
  }
}

/*
  This function was purely made for testing purposes.
  It will print the blocksizes and addresses of the 
  free linked list starting from the tail pointer.
*/

void printLLBack() {
  mmalloc * temp = fTail;
  while (temp != NULL) {
    printf("**********%p, %lu**********\n", temp, temp->blockSize);
    temp = temp->prev;
  }
}

int main() {
  int ** newPtr = (int **)bf_malloc(40 * sizeof(int *));
  for (size_t i = 0; i < 40; i++) {
    printf("Mallocing %lu number of bytes\n", ((4 * (i + 1) * sizeof(int))));
    newPtr[i] = (int *)bf_malloc((4 * (i + 1)) * sizeof(int));
  }

  for (size_t i = 0; i < 40; i++) {
    bf_free(newPtr[i]);
  }
  bf_free(newPtr);
  /* bf_free(newPtr[0]); */
  /* bf_free(newPtr[2]); */
  /* bf_free(newPtr[3]); */

  /* bf_free(newPtr[1]); */
  /* bf_free(newPtr); */
  printLLFront();
}
