#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "half_fit.h"

UnallocatedBlock_t* BucketArray[11] = {};
static char array[ARR_SZ];
//The base address. On keil this would be 0x10000000, but on our computer we have to use a dynamic one
void *p_start = array;

//rewrite
int ceil_log2(unsigned long long x)
{
  static const unsigned long long t[6] = {
    0xFFFFFFFF00000000ull,
    0x00000000FFFF0000ull,
    0x000000000000FF00ull,
    0x00000000000000F0ull,
    0x000000000000000Cull,
    0x0000000000000002ull
  };

  int y = (((x & (x - 1)) == 0) ? 0 : 1);
  int j = 32;
  int i;

  for (i = 0; i < 6; i++) {
    int k = (((x & t[i]) == 0) ? 0 : j);
    y += k;
    x >>= k;
    j >>= 1;
  }

  return y;
}

void PushToBucket(UnallocatedBlock_t* pointer){
    int bucketNum = ceil_log2(pointer);
    if(bucketNum < 0 || bucketNum >= 11)
}

UnallocatedBlock_t* PopBucket (int chunks){
    int bucketNum = ceil_log2(chunks) + 1; // +1 because you take the guaranteed bucket
    if(bucketNum < 0 || bucketNum >= 11)
        return NULL;
    while(BucketArray[bucketNum] == NULL) {
        bucketNum = bucketNum + 1;
        if (bucketNum >= 11)
            return NULL;
    }
    

}

void  half_init( void ){
    int i = 0;
    UnallocatedBlock_t* header = (UnallocatedBlock_t*)p_start;
    header->Header.Allocated = 0;
    header->Header.NextBlock = 0;
    header->Header.PrevBlock = 0;
    header->Header.Size = 0;
    header->NextFree = 0;
    header->PrevFree = 0;
    for(;i < 11; i++){
        BucketArray[i] = NULL;
    }
}

/**********
size is the number of bytes that the user wants to allocate


**********/
void *half_alloc( int size ){
    size += 4; // 4 bytes are required for header
    int numChunk = (size+31)/32; // this divide size by 32 and ceil it REWRITE IN BITWISE
    UnallocatedBlock_t * freeBlock = PopBucket(numChunk);

}

void  half_free( void * pointer){

}

