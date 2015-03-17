#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"


#define SIZE 4096 * 64


/* Simple test for smalloc and sfree. */

int main(void) {

    mem_init(SIZE);
    
    char *ptrs[10];
    int i;

    /* Call smalloc 10 times */
	/* This is so that the smalloc function can run for many number of calls */
    
    for(i = 0; i < 9; i++) {
        int num_bytes = (i+1) * 10;
    
        ptrs[i] = smalloc(num_bytes);
        write_to_mem(num_bytes, ptrs[i], i);
    }
	
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();
    
	/* Choosing an arbritary index deep inside the allocated_list so that we know that sfree is able to function for larger iterations*/
    printf("freeing %p result = %d\n", ptrs[7], sfree(ptrs[7]));
    
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();
	
    mem_clean();
    return 0;
}
