#include <stdio.h>
#include <unistd.h>

typedef struct My_malloc {
  struct My_malloc * next;
  struct My_malloc * prev;
  size_t blockSize;
} mmalloc;

//This function is the substitute of malloc. Probably FF
void * bf_malloc(size_t noBytes);
//This function was purely made for testing purposes.
void printLLFront(mmalloc * headPtr);
//This function is the function used to free a memory region
void bf_free(void * ptrToDelete);
//This function is also for testing
void printLLBack(mmalloc * tailPtr);
//helper function for adding to free linked linst
void addSortedFree(mmalloc * ptrToAdd);
//Merges adjacent free regions
void mergeFreeList();
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
void * reusebf_malloc(size_t noBytes);
void addSortedFree(mmalloc * ptrToAdd);
void printLLFront(mmalloc * headPtr);
void printLLBack(mmalloc * tailPtr);
void purify(mmalloc * result);
void addToMalloc(mmalloc * newNode);
void * ff_malloc(size_t noBytes);
void ff_free(void * newPtr);
