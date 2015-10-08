#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "half_fit.h"

UnallocatedBlock_t *BucketArray[11] = {};
int LowerLimit[11] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
static char array[ARR_SZ];
//The base address. On keil this would be 0x10000000, but on our computer we have to use a dynamic one
void *p_start = array;

void print(void){
    void*a = p_start;
    UnallocatedBlock_t* a0 = BucketArray[0];
    UnallocatedBlock_t* a1 = BucketArray[1];
    UnallocatedBlock_t* a2 = BucketArray[2];
    UnallocatedBlock_t* a3 = BucketArray[3];
    UnallocatedBlock_t* a4 = BucketArray[4];
    UnallocatedBlock_t* a5 = BucketArray[5];
    UnallocatedBlock_t* a6 = BucketArray[6];
    UnallocatedBlock_t* a7 = BucketArray[7];
    UnallocatedBlock_t* a8 = BucketArray[8];
    UnallocatedBlock_t* a9 = BucketArray[9];
    UnallocatedBlock_t* a10 = BucketArray[10];
}

int inline ceil_log2(unsigned long long x) {
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


unsigned int inline GetRelativeAddress(void *pointer) {
    unsigned int relativeAddress = (unsigned int) ((pointer - p_start) / 32);
    return relativeAddress;
}


inline void *GetAbsoluteAddress(unsigned int relativeAddress) {
    int AbsoluteAddress = (int) ((relativeAddress * 32) + p_start);
    return AbsoluteAddress;
}

//this function assume the allocated header is set correctly
void inline PushToBucket(UnallocatedBlock_t *pointer) {
    unsigned int size = pointer->Header.Size;
    if (size == 0)
        size = 1024;
    int bucketNum = ceil_log2(size);
    if (bucketNum < 0 || bucketNum >= 11)
        return;//bucket out of range

    if (BucketArray[bucketNum] == NULL) { //if bucket is empty, set it to the current one
        BucketArray[bucketNum] = pointer;
    }

    pointer->NextFree = GetRelativeAddress(BucketArray[bucketNum]);//set next free to the old first block in bucket or itself if it was empty
    pointer->PrevFree = GetRelativeAddress(pointer);//set prev free to itself to indicate nothing
}

inline UnallocatedBlock_t *PopBucket(int chunks) {
    int bucketNum = ceil_log2((unsigned long long int) chunks);
    if (bucketNum < 0 || bucketNum >= 11)
        return NULL;
    if(chunks > LowerLimit[bucketNum])
        bucketNum++;// +1 because you take the guaranteed bucket
    if (bucketNum < 0 || bucketNum >= 11)
        return NULL;
    while (BucketArray[bucketNum] == NULL) {
        bucketNum = bucketNum + 1;
        if (bucketNum >= 11)
            return NULL;
    }
    UnallocatedBlock_t *ptrToBucketNum = BucketArray[bucketNum];
    RemoveBlockFromBucket(BucketArray[bucketNum]);
    return ptrToBucketNum;
}

void half_init(void) {
    int i = 0;
    UnallocatedBlock_t *header = (UnallocatedBlock_t *) p_start;
    header->Header.Allocated = 0;
    header->Header.NextBlock = 0;
    header->Header.PrevBlock = 0;
    header->Header.Size = 0;
    header->NextFree = 0;
    header->PrevFree = 0;
    for (; i < 11; i++) {
        BucketArray[i] = NULL;
    }
    BucketArray[10] = p_start;
}


AllocatedBlock_t inline *splitBlock(UnallocatedBlock_t *freeBlock, int numChunk) {
    unsigned int sizeFreeTotal = freeBlock->Header.Size;
    if (sizeFreeTotal == 0)
        sizeFreeTotal = 1024;
    AllocatedBlock_t *allocBlock = (AllocatedBlock_t *) freeBlock;
    allocBlock->Size = (unsigned int) numChunk;
    allocBlock->Allocated = 1;

    freeBlock = GetAbsoluteAddress(GetRelativeAddress(freeBlock) + numChunk); // changing freeBlock from the original
    // sized block to the second (freeBlock) when split
    freeBlock->Header.Size = (sizeFreeTotal - allocBlock->Size);
    freeBlock->Header.PrevBlock = GetRelativeAddress(allocBlock);
    if (allocBlock->NextBlock == GetRelativeAddress((void *) allocBlock))
        freeBlock->Header.NextBlock = GetRelativeAddress((void *) freeBlock);
    else
        freeBlock->Header.NextBlock = allocBlock->NextBlock;
    allocBlock->NextBlock = GetRelativeAddress(freeBlock);
    PushToBucket(freeBlock);
    return allocBlock;
}

/**********
size is the number of bytes that the user wants to allocate


**********/
void *half_alloc(int size) {
    print();
    size += 4; // 4 bytes are required for header
    int numChunk = (size + 31) / 32; // this divide size by 32 and ceil it REWRITE IN BITWISE
    UnallocatedBlock_t *freeBlock = PopBucket(numChunk);
    if (freeBlock == NULL)
        return NULL;
    if (freeBlock->Header.Size == numChunk || (freeBlock->Header.Size+1024) == numChunk) {
        freeBlock->Header.Allocated = 1;
        return (AllocatedBlock_t*)(((void*)freeBlock)+4);
    }
    AllocatedBlock_t* allocBlock = splitBlock(freeBlock, numChunk);
    return (AllocatedBlock_t*)(((void*)allocBlock)+4);
}


void inline RemoveBlockFromBucket(UnallocatedBlock_t *pointer) {
    unsigned int size = pointer->Header.Size;
    if (size == 0)
        size = 1024;
    int bucketNum = ceil_log2(size);
    unsigned int CurrentRelative = GetRelativeAddress(pointer);
    //assign prevfree's next free pointer
    if (pointer->PrevFree == CurrentRelative) {// popping the first one on the list. there is no prev free
        BucketArray[bucketNum] = (UnallocatedBlock_t *) GetAbsoluteAddress(pointer->NextFree);
        if (BucketArray[bucketNum] == pointer) { // only one thing in the bucket
            BucketArray[bucketNum] = NULL;
        } else {
            BucketArray[bucketNum]->PrevFree = pointer->NextFree; //point it to itself so to indicate there is nothing before it
        }
    }
    else if (pointer->NextFree == CurrentRelative) {//popping the last one on the list and there is more than on in the list
        ((UnallocatedBlock_t *) GetAbsoluteAddress(pointer->PrevFree))->NextFree = pointer->PrevFree;
    }
    else {
        ((UnallocatedBlock_t *) GetAbsoluteAddress(pointer->PrevFree))->NextFree = pointer->PrevFree;
        ((UnallocatedBlock_t *) GetAbsoluteAddress(pointer->NextFree))->PrevFree = pointer->NextFree;
    }
}

void half_free(void *pointer) {
    print();
    UnallocatedBlock_t *CurrentBlockHeader = (UnallocatedBlock_t *) (pointer - 4);
    if(!CurrentBlockHeader->Header.Allocated)
        return;
    unsigned int size = CurrentBlockHeader->Header.Size;
    if (size == 0)
        size = 1024;
    UnallocatedBlock_t *FirstCoalesceBlock = CurrentBlockHeader;
    UnallocatedBlock_t *LastCoalesceBlock = CurrentBlockHeader;
    UnallocatedBlock_t *PrevBlock = (UnallocatedBlock_t *) GetAbsoluteAddress(CurrentBlockHeader->Header.PrevBlock);
    UnallocatedBlock_t *NextBlock = (UnallocatedBlock_t *) GetAbsoluteAddress(CurrentBlockHeader->Header.NextBlock);
    UnallocatedBlock_t *NextBlockAfterCoalesce = NULL;

    if (!PrevBlock->Header.Allocated && PrevBlock != CurrentBlockHeader) {
        FirstCoalesceBlock = PrevBlock;
        size += PrevBlock->Header.Size;
        RemoveBlockFromBucket(PrevBlock);
    }
    if (!NextBlock->Header.Allocated && NextBlock != CurrentBlockHeader) {
        LastCoalesceBlock = NextBlock;
        size += NextBlock->Header.Size;
        RemoveBlockFromBucket(NextBlock);
    }

    //Prev block should be already set
    FirstCoalesceBlock->Header.Allocated = 0;
    FirstCoalesceBlock->Header.Size = (unsigned int) size;

    FirstCoalesceBlock->Header.NextBlock = LastCoalesceBlock->Header.NextBlock;
    if (LastCoalesceBlock->Header.NextBlock == GetRelativeAddress(LastCoalesceBlock)) {//there is no nextblock
        FirstCoalesceBlock->Header.NextBlock = GetRelativeAddress(FirstCoalesceBlock);
    }
    else {
        NextBlockAfterCoalesce = GetAbsoluteAddress(LastCoalesceBlock->Header.NextBlock);
        NextBlockAfterCoalesce->Header.PrevBlock = GetRelativeAddress(FirstCoalesceBlock);
    }
    PushToBucket(FirstCoalesceBlock);
    print();
}
