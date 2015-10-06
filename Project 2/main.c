#include <stdio.h>
#include <stdlib.h>
#include "half_fit.h"


int main(void){
    printf("%d\n",sizeof(UnallocatedBlock_t));
    printf("%d\n",sizeof(AllocatedBlock_t));

/*
AllocatedBlock_t* theAddress;
theAddress->Allocated;
void* p_start = NULL;
((AllocatedBlock_t*) p_start)->Size;
AllocatedBlock_t* block = (AllocatedBlock_t*) p_start;
block->PrevBlock = (theAddress-0x10000000)/32
block->PrevBlock;//int 0 - 1024
AllocatedBlock_t* hahah= (block->PrevBlock)*32+0x10000000;
hahah->
*/
}
