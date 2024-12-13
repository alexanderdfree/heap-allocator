// A segregated storage memory allocator.  Complete the code required in the functions below
// to implement a segregated memory allocator.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "alloc.h"

int max_heap_size = 1024;  // default is a 1K heap (pretty small)

void *heap[4] = {NULL,NULL,NULL,NULL}; // the heap bins for each segregated size 8,16,32,64

unsigned int heap_size = 0; // A variable that keeps track of total size of the heap

// allocate_slab uses the default malloc library to allocate memory
// input: size in bytes of slab to allocate
// return: pointer to slab allocated, NULL if allocation would cause the heap to exceed max_heap_size
void *allocate_slab(unsigned int size) {
    if (size+heap_size > max_heap_size) return NULL;
    void* s = malloc(size);
    heap_size += size;
    if (s==NULL) return NULL;
    return s;
}

// Returns a pointer to a heap block of the appropriate
// size.
// If no suitable block exists in the selected bin's free list, this function calls
// allocate_slab to get space NUM_HEAP_BLOCKS blocks of the
// bin's defined size and places them on the bin's free list, then returns
// one of them.
void *my_alloc(unsigned int size) {
    //  If the bin's free list is not empty, then it returns the first block on the free list, removing it from the free list.
    //Important! If the size argument to my_alloc has a value larger than the largest bin block size, 
    //your code should printf "Malloc failed, out of memory!\n" and then simply call exit(0)
    int bin = -1;
    int binSize = -1;
    if (size <= 8){
        if (heap[0] != NULL) {
            void* r = heap[0];
            heap[0] = ((blk_header_t*) (heap[0]))->next;
            return (void *)((char*)r + sizeof(blk_header_t));
        }
        bin = 0;
        binSize = 8;
    }
    else if (size <= 16){
        if (heap[1] != NULL) {
            void* r = heap[1];
            heap[1] = ((blk_header_t*) (heap[1]))->next;
            return (void *)((char*)r + sizeof(blk_header_t));
        }
        bin = 1;
        binSize = 16;
    }
    else if (size <= 32){
        if (heap[2] != NULL) {
            void* r = heap[2];
            heap[2] = ((blk_header_t*) (heap[2]))->next;
            return (void *)((char*)r + sizeof(blk_header_t));
        }
        bin = 2;
        binSize = 32;
    }
    else if (size <= 64){
        if (heap[3] != NULL) {
            void* r = heap[3];
            heap[3] = ((blk_header_t*) (heap[3]))->next;
            return (void *)((char*)r + sizeof(blk_header_t));
        }
        bin = 3;
        binSize = 64;
    }
    else{
        printf("Malloc failed, out of memory!\n");
        exit(0);
    }
    
    //Else if the free list for the selected bin is empty, call allocate_slab to get a new slab to refill that bin.   The slab should be large enough to carve it into NUM_HEAP_BLOCKS of the bin's block size, together with their headers.  Refer to the diagram.   
    //Carve the slab, initialiize the block headers, and place those new blocks on the bin's free list.  
    //This is the trickiest part of the lab:  it involves some pointer arithmetic and type casting.   
    //Be mindful that arithmetic on a T* pointer is implicitly in units of sizeof(T), 
    //while arithmetic on a void* pointer is in units of bytes.   Note: NUM_HEAP_BLOCKS is defined in alloc.h.
    //heap_size+=size;

    size_t slabSize = NUM_HEAP_BLOCKS * (binSize + sizeof(blk_header_t));
    void *slab = allocate_slab(slabSize);
    if (slab == NULL){
        printf("Malloc failed, out of memory!\n");
        exit(0);
    }
    void *curr = slab;
    for (int i = 0; i < NUM_HEAP_BLOCKS; i++) {
            blk_header_t *header = (blk_header_t *)curr;
            //header->next = NULL;
            //header->size = NULL;

            header->size = binSize;
            
            if (i == NUM_HEAP_BLOCKS - 1) header->next = NULL;
            else header->next = (blk_header_t *)((char*)curr + sizeof(blk_header_t) + binSize);

            curr = (void *)((char*)curr + sizeof(blk_header_t) + binSize);
        }

    heap[bin] = slab;
    blk_header_t* b = heap[bin];
    heap[bin] = b->next;
    return (void *)((char*)b + sizeof(blk_header_t));
}
/*
This function mimics the free routine in the C standard library.   
The argument is a pointer value previously returned from my_alloc.  
It is a pointer to a block, i.e., to the first byte of the block itself, and not the block's header!   
Insert the block (or more precisely, its header) into the front of the free list of the bin for its size.   
Select the right bin for the block's size, which is in its header.     
(Hint: use pointer arithmetic to get a pointer to the block's header.)
*/
void my_free(void *ptr) {
    //header address is *ptr-16 or *ptr-sizeof(block header type);
    int bin = -1;
    //int binSize = -1;
    if (ptr == NULL){
        printf("Free failed!\n");
        exit(0);
    }
    blk_header_t* ptr2 = (blk_header_t *)((char *)ptr - sizeof(blk_header_t));
    int size = ptr2->size;

    if (size <= 8){
        bin = 0;
        //binSize = 8;
    }
    else if (size <= 16){
        
        bin = 1;
        //binSize = 16;
    }
    else if (size <= 32){
        
        bin = 2;
        //binSize = 32;
    }
    else if (size <= 64){
        
        bin = 3;
        //binSize = 64;
    }

    ptr2->next = heap[bin];
    heap[bin] = ptr2;
    //return ptr2;
}
