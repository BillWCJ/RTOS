#include <stdio.h>
#include <stdlib.h>
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

unsigned int GetRelativeAddress(void* pointer){
    unsigned int relativeAddress = (pointer-p_start)/32;
    return relativeAddress;
}

void* GetAbsoluteAddress (unsigned int relativeAddress){
    int AbsoluteAddress = (relativeAddress*32)+p_start;
    return (void*)AbsoluteAddress;
}

//this function assume the allocated header is set correctly
void PushToBucket(UnallocatedBlock_t* pointer){
    int bucketNum = ceil_log2(pointer->Header.Size);
    if(bucketNum < 0 || bucketNum >= 11)
        return;//bucket out of range

    if(BucketArray[bucketNum] == NULL){ //if bucket is empty, set it to the current one
        BucketArray[bucketNum] = pointer;
    }

    pointer->NextFree = GetRelativeAddress(BucketArray[bucketNum]);//set next free to the old first block in bucket or itself if it was empty
    pointer->PrevFree = GetRelativeAddress(pointer);//set prev free to itself to indicate nothing
}
UnallocatedBlock_t* PopBucket (int chunks){

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
    int chunks = (size+31)/32; // this divide size by 32 and ceil it to get chunks
    UnallocatedBlock_t* newBlock = PopBucket(chunks);
    if(newBlock == NULL)
        return NULL;

}

void  half_free( void * pointer){

}

