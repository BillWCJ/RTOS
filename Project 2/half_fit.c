#include <stdio.h>
#include <stdlib.h>
#include "half_fit.h"

UnallocatedBlock_t* BucketArray[11] = {};
static char array[ARR_SZ];
//The base address. On keil this would be 0x10000000, but on our computer we have to use a dynamic one
void *p_start = array;


void  half_init( void ){

}

/**********
size is the number of bytes that the user wants to allocate


**********/
void *half_alloc( int size ){
    size += 4; // 4 bytes are required for header
    int numChunk = (size+31)/32; // this divide size by 32 and ceil it REWRITE IN BITWISE




}

void  half_free( void * pointer){


}
