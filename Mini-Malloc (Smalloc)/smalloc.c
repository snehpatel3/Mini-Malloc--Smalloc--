#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"



void *mem;
struct block *freelist;
struct block *allocated_list;

// smalloc function 
void *smalloc(unsigned int nbytes) {

// declaring a pointer to iterate through the freelist
	struct block *b_pointer = freelist; 
	
// iteration begins
	while (b_pointer != NULL){
	// check to see if the the size of the current node in freelist is sufficent for nbytes
		if(b_pointer->size >= nbytes){
			struct block * node = malloc(sizeof(struct block));
			node->addr = b_pointer->addr; 
			node->size = nbytes; 
			node->next = allocated_list; 
			allocated_list = node; 	// note: here is where allocated_list gets a value
			b_pointer->size = freelist->size - nbytes; 
			b_pointer->addr = freelist->addr + nbytes;
			break; 
		}
		else{
		b_pointer = b_pointer->next; 		// move to next if it's not sufficient 
		}
	}
	// return the address of the stored nbytes
	return allocated_list->addr; 
}

// sfree function
int sfree(void *addr) {

	// declare a whole bunch of pointers used for iterating
	struct block *f_pointer = allocated_list; 
	struct block *free_ptr = freelist; 
	
	// declare a whole bunch of pointers used for storing previous node values 
	struct block *pre_free = NULL; 
	struct block *p_pointer = NULL; 
	
	// iteration begins
	while(f_pointer != NULL){ 
	// check to see if the node is the one we want 
	if(f_pointer->addr == addr){
		if(p_pointer == NULL){
			allocated_list = allocated_list->next; 
		}else{
		p_pointer->next = f_pointer->next;  
		}
		
		if(freelist == NULL){
		f_pointer->next = NULL; 
		freelist = f_pointer; 
		return(0); 
		}
	
		while(free_ptr != NULL){
		
		if(f_pointer->addr < free_ptr->addr){
			if(pre_free == NULL){
			f_pointer->next = free_ptr; 
			freelist = f_pointer; 
			}else{
			pre_free->next = f_pointer; 
			f_pointer->next = free_ptr; 
			}
			return(0); 
		}else{
			free_ptr = free_ptr->next; 
			}
		}
		if(p_pointer->next == NULL){
			p_pointer->next = f_pointer; 
			f_pointer->next = NULL; 
		} 
	}
	else{
		p_pointer = f_pointer; 
		f_pointer = f_pointer->next;  // move on to the next node if it's not found 
	}
	}
	return(-1); 
}


/* Initialize the memory space used by smalloc,
 * freelist, and allocated_list
 * Note:  mmap is a system call that has a wide variety of uses.  In our
 * case we are using it to allocate a large region of memory. 
 * - mmap returns a pointer to the allocated memory
 * Arguments:
 * - NULL: a suggestion for where to place the memory. We will let the 
 *         system decide where to place the memory.
 * - PROT_READ | PROT_WRITE: we will use the memory for both reading
 *         and writing.
 * - MAP_PRIVATE | MAP_ANON: the memory is just for this process, and 
 *         is not associated with a file.
 * - -1: because this memory is not associated with a file, the file 
 *         descriptor argument is set to -1
 * - 0: only used if the address space is associated with a file.
 */
 
// mem_init function
void mem_init(int size) {
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
    } 
	// initalizing freelist here 
    freelist = malloc(sizeof(struct block)); 
	freelist->addr = mem; 
	freelist->size = size; 
	// note: freelist->next is null because we only have 1 block 
	freelist->next = NULL; 
	// note: allocated_list is null for now will be assigned to something in smalloc function
	allocated_list = NULL;
}

// mem_clean function 
void mem_clean(){
	// declaring temporary pointers to store previous nodes 
	struct block * temp_allocated;
	struct block * temp_freed;
	
	// temp_allocated is used to store in order to free other blocks without memory leak 
	while(allocated_list != NULL){
		temp_allocated = allocated_list; 
		allocated_list = allocated_list->next; 
		free(temp_allocated);
	}
	
	// temp_freed is used to store in order to free other blocks without memory leak 
	while(freelist != NULL){
		temp_freed = freelist; 
		freelist = freelist->next; 
		free(temp_freed); 
	}
}
