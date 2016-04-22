#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "half_fit.h"

unsigned char array[MAX_SIZE] __attribute__ ((section(".ARM.__at_0x10000000"), zero_init));

UnallocatedBlock_t* BucketArray[11] = {0,0,0,0,0,0,0,0,0,0,0};
const int LowerLimit[11] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
//#define p_start 0x10000000

// The commented out functions are for debuging purposes only

//static char array[ARR_SZ];
//void *p_start = array;

/****
Go to the begining of p_start adn traverse through next pointer, once the last is reached
go backwards and make sure the two equal in terms of number of blocks 
****/
// void check(void){
//     int numblock = 0;
//     bool lala = true;
//     AllocatedBlock_t* temp = p_start;
//     while(temp <= (p_start+(1024*32))){
//         numblock++;
//         if(temp->NextBlock == 0 && !lala)
//             break;
//         temp = GetAbsoluteAddress(temp->NextBlock);
//     }
//     int num2 = CheckPrev(temp);
//     if(numblock != num2)
//         printf("-----------%d != %d\n", numblock, num2);
// }

/****
Called by check
Count number of blocks backwards using reccursion, and prev pointer
****/
// int CheckPrev(AllocatedBlock_t* pointer){
//     if(pointer == p_start){
//             return 1;
//     }
//     else{
//         return CheckPrev(GetAbsoluteAddress(pointer->PrevBlock)) + 1;
//     }
// }

/****
Print out info such as:
Buckets
Block indexies - forward and backward
****/
// void print(void) {
//     void*a = (void *) p_start;
//     UnallocatedBlock_t* a0 = BucketArray[0];
//     UnallocatedBlock_t* a1 = BucketArray[1];
//     UnallocatedBlock_t* a2 = BucketArray[2];
//     UnallocatedBlock_t* a3 = BucketArray[3];
//     UnallocatedBlock_t* a4 = BucketArray[4];
//     UnallocatedBlock_t* a5 = BucketArray[5];
//     UnallocatedBlock_t* a6 = BucketArray[6];
//     UnallocatedBlock_t* a7 = BucketArray[7];
//     UnallocatedBlock_t* a8 = BucketArray[8];
//     UnallocatedBlock_t* a9 = BucketArray[9];
//     UnallocatedBlock_t* a10 = BucketArray[10];
//     int i = 0;
//     AllocatedBlock_t* temp = (AllocatedBlock_t*) p_start;
//     bool lala = true;
//     bool startPrint = false;
//     while(temp <= ((AllocatedBlock_t*)p_start+(1024*32))){
//         unsigned int location = GetRelativeAddress(temp);
//         if(temp->Allocated==0)
//             printf("%d!",GetRelativeAddress(temp));
//         else
//             printf("%d.",GetRelativeAddress(temp));

//         if(temp->NextBlock == 0 && !lala)
//             break;
//         temp = (AllocatedBlock_t*) GetAbsoluteAddress(temp->NextBlock);

//         lala = false;
//     }
//     printf("\n");
//     PrintPrev(temp);
//     printf("\n");
//     for (; i < 11; i++){
//        if(BucketArray[i] ==NULL)
//             printf("a%d:XXXXX",i);
//        else
//             printf("a%d:%5d",i,(((int)BucketArray[i]-(int)p_start)+31)/32);
//     }
//     printf("\n");
// }


/****
Prints the blocks -backwards 
****/
// void PrintPrev(AllocatedBlock_t* pointer) {
//     if((int)pointer == p_start){
//         if(pointer->Allocated==0)
//             printf("%d!",GetRelativeAddress(pointer));
//         else
//             printf("%d.",GetRelativeAddress(pointer));
//     }
//     else{
//         PrintPrev(GetAbsoluteAddress(pointer->PrevBlock));
//         if(pointer->Allocated==0)
//             printf("%d!",GetRelativeAddress(pointer));
//         else
//             printf("%d.",GetRelativeAddress(pointer));
//     }
// }

/****
Converts the absolute address into relative address  
****/
unsigned int GetRelativeAddress(void *pointer) {
 	return (unsigned int)((unsigned int)pointer - ((unsigned int) p_start))>>5;
}

/****
Converts the relative address into absolute address  
****/
__inline void* GetAbsoluteAddress(unsigned int relativeAddress) {
    return (void*)((relativeAddress<<5) + p_start);
}

/****
Takes the log(base 2) of a number using bitwise
****/
int log2(unsigned int size) {
    int count = 0;
    while(size >>= 1) count++;
    return count;// + 1
}

/****
Returns the correct size of the block 
****/
unsigned int GetSize(unsigned int size) {
    if (size == 0) size = 1024;
    return size;
}

/****
Finds the correct bucket based on size, and append the block into the doubly linked list
****/
void PushToBucket(UnallocatedBlock_t *pointer) {
    int bucketNum = log2(GetSize(pointer->Header.Size));
    if(BucketArray[bucketNum] != NULL) {
        BucketArray[bucketNum]->PrevFree = pointer;
    }
    pointer->NextFree = BucketArray[bucketNum];
    pointer->PrevFree = NULL;
    BucketArray[bucketNum] = pointer;
}

/****
Remove the reference of an allocated block from the bucket array
****/
UnallocatedBlock_t* RemoveBlockFromBucket(UnallocatedBlock_t *pointer) {
    int bucketNum = log2(GetSize(pointer->Header.Size));
    //assign prevfree's next free pointer
    if(BucketArray[bucketNum] == pointer) {// popping the first one on the list. there is no prev free
        BucketArray[bucketNum] = pointer->NextFree;
        if(BucketArray[bucketNum] != NULL)// only one thing in the bucket
            BucketArray[bucketNum]->PrevFree = NULL;
    }
    else {
        if(pointer->PrevFree != NULL)
            pointer->PrevFree->NextFree = pointer->NextFree;
        if(pointer->NextFree != NULL)
            pointer->NextFree->PrevFree = pointer->PrevFree;
    }
	return pointer;
}

/****
Return an unallocated block from the bucket
Uses RemoveBlockFromBucket to remove the references 
****/
__inline UnallocatedBlock_t *PopBucket(int chunks) {
    int bucketNum = log2((unsigned int) chunks);
    if(chunks > LowerLimit[bucketNum])
        bucketNum++;// +1 because you take the guaranteed bucket
    if(bucketNum < 0 || bucketNum >= 11) // check if its outside the bucket array range
        return NULL; 
    while(BucketArray[bucketNum] == NULL) { 
        bucketNum++;
        if(bucketNum >= 11) // check if its outside the bucket array range
            return NULL;
    }
    return RemoveBlockFromBucket(BucketArray[bucketNum]);
}

/****
Initializing the header for the entire memory block
****/
void half_init(void) {
    int i = 0;
    UnallocatedBlock_t* header = (UnallocatedBlock_t*) p_start;
    header->Header.Allocated = 0;
    header->Header.NextBlock = 0;
    header->Header.PrevBlock = 0;
    header->Header.Size = 0;
    header->Header.LastBlock = 1;
    header->NextFree = NULL;
    header->PrevFree = NULL;
    for (; i < 10; i++)
        BucketArray[i] = NULL;
    BucketArray[10] = header;
}

/****
Takes an unallocated block and splits it into two, one being allocated and the other unallocated
Returns the reference to the allocated
Reinserts the unallocated block into the bucket array based on size remaining 
****/
__inline AllocatedBlock_t *splitBlock(UnallocatedBlock_t *freeBlock, int numChunk) {
    UnallocatedBlock_t* SecondSplitBlock = (UnallocatedBlock_t*)((int)freeBlock + (numChunk*32));
	unsigned int SecondSplitedBlockRelAddress = GetRelativeAddress(SecondSplitBlock);
    unsigned int sizeFreeTotal = GetSize(freeBlock->Header.Size);

    AllocatedBlock_t* allocBlock = (AllocatedBlock_t *) freeBlock;
    allocBlock->Size = (unsigned int) numChunk;
    allocBlock->Allocated = 1;

    // changing freeBlock from the original
    // sized block to the second (freeBlock) when split
    SecondSplitBlock->Header.Size = (unsigned int)(sizeFreeTotal - numChunk);
    SecondSplitBlock->Header.PrevBlock = GetRelativeAddress(allocBlock);
    SecondSplitBlock->Header.Allocated = 0;
    if(allocBlock->LastBlock) {
        allocBlock->LastBlock = 0;
        SecondSplitBlock->Header.LastBlock = 1;
        SecondSplitBlock->Header.NextBlock = 0;
    }
    else {
        ((AllocatedBlock_t*) GetAbsoluteAddress(allocBlock->NextBlock))->PrevBlock = SecondSplitedBlockRelAddress;
        SecondSplitBlock->Header.NextBlock = allocBlock->NextBlock;
        SecondSplitBlock->Header.LastBlock = 0;
    }

    allocBlock->NextBlock = SecondSplitedBlockRelAddress;

    PushToBucket(SecondSplitBlock);
    return allocBlock;
}

/****
size is the number of bytes that the user wants to allocate
return a pointer to the usable memory of the allocated block (address without header)
****/
void *half_alloc(int size) {
	UnallocatedBlock_t* freeBlock = NULL;
    if (size <= 0 || size >32764)
        return NULL;
 		//size += 4; // 4 bytes are required for header
 		//size = (size + 31) / 32; // this divide size by 32 and ceil it 
 		size += 35;
		size>>=5;
		
    freeBlock = PopBucket(size);
    if (freeBlock == NULL)
        return NULL;
    if (GetSize(freeBlock->Header.Size)== size) {
        freeBlock->Header.Allocated = 1;
        return (AllocatedBlock_t*)(((unsigned int)freeBlock)+4);
    }
    return (AllocatedBlock_t*)(((unsigned int)splitBlock(freeBlock, size))+4);
}

/****
frees the block of memory specified by the user
it checks the prev block and next block, and if any or all are free it coaleses (joins) them together and pushes to 
the approprate bucket array spot based on new free size
****/
void half_free(void *pointer) {
    UnallocatedBlock_t* CurrentBlockHeader = (UnallocatedBlock_t *) ((unsigned int)pointer - 4);
    unsigned int size = GetSize(CurrentBlockHeader->Header.Size);
	UnallocatedBlock_t* FirstCoalesceBlock = CurrentBlockHeader;
    UnallocatedBlock_t* LastCoalesceBlock = CurrentBlockHeader;
    UnallocatedBlock_t* PrevBlock = (UnallocatedBlock_t *) GetAbsoluteAddress(CurrentBlockHeader->Header.PrevBlock);
    UnallocatedBlock_t* NextBlock = (UnallocatedBlock_t *) GetAbsoluteAddress(CurrentBlockHeader->Header.NextBlock);
    AllocatedBlock_t* NextBlockAfterCoalesce = NULL;

		//if prev is coalesable 
    if (PrevBlock->Header.Allocated==0 && (int)CurrentBlockHeader != p_start && PrevBlock != CurrentBlockHeader ) {
        FirstCoalesceBlock = PrevBlock;
        size += PrevBlock->Header.Size;
        RemoveBlockFromBucket(PrevBlock);
    }
		//if next is coalesable 
    if (NextBlock->Header.Allocated == 0 && (int)NextBlock != p_start) {
        LastCoalesceBlock = NextBlock;
        size += NextBlock->Header.Size;
        RemoveBlockFromBucket(NextBlock);
    }

    //Prev block should be already set
    FirstCoalesceBlock->Header.Allocated = 0;
    FirstCoalesceBlock->Header.Size = size;
		
    //reseting header values for the new free sized block 
    FirstCoalesceBlock->Header.NextBlock = LastCoalesceBlock->Header.NextBlock;
    if (LastCoalesceBlock->Header.LastBlock) { //there is no next block
        FirstCoalesceBlock->Header.NextBlock = 0;
        FirstCoalesceBlock->Header.LastBlock = 1;
    }
    else {
        NextBlockAfterCoalesce = GetAbsoluteAddress(FirstCoalesceBlock->Header.NextBlock);
        NextBlockAfterCoalesce->PrevBlock = GetRelativeAddress(FirstCoalesceBlock);
    }
    PushToBucket(FirstCoalesceBlock);
}
