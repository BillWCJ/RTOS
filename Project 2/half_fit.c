#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "half_fit.h"

UnallocatedBlock_t* BucketArray[11] = {0,0,0,0,0,0,0,0,0,0,0};
const int LowerLimit[11] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
//The base address. On keil this would be 0x10000000, but on our computer we have to use a dynamic one
//#define p_start 0x10000000
//unsigned char array[MAX_SIZE] __attribute__ ((section(".ARM.__at_0x10000000"), zero_init));
static char array[ARR_SZ];
void *p_start = array;

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

// void PrintPrev(AllocatedBlock_t* pointer){
//     if(pointer == p_start){
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

// int CheckPrev(AllocatedBlock_t* pointer){
//     if(pointer == p_start){
//             return 1;
//     }
//     else{
//         return CheckPrev(GetAbsoluteAddress(pointer->PrevBlock)) + 1;
//     }
// }
// void print(void){
//     void*a = p_start;
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
//     AllocatedBlock_t* temp = p_start;
//     bool lala = true;
//     bool startPrint = false;
//     while(temp <= (p_start+(1024*32))){
//         int location = GetRelativeAddress(temp);
//         if(temp->Allocated==0)
//             printf("%d!",GetRelativeAddress(temp));
//         else
//             printf("%d.",GetRelativeAddress(temp));

//         if(temp->NextBlock == 0 && !lala)
//             break;
//         temp = GetAbsoluteAddress(temp->NextBlock);

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

int ceil_log2(unsigned long long x) {
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


unsigned int GetSize(unsigned int size){
    if (size == 0)
        size = 1024;
    return size;
}

unsigned int GetBucketNumOfAGivenSize(unsigned int size){
    int bucketNum = ceil_log2(size)-1;
    if(bucketNum < 0)
        bucketNum++;
    if(size > 2*LowerLimit[bucketNum]-1)
        bucketNum++;
    return bucketNum;
}

unsigned int GetRelativeAddress(void *pointer) {
    unsigned int relativeAddress = (unsigned int) (((unsigned int)pointer - (unsigned int)p_start) / 32);
    return relativeAddress;
}


void* GetAbsoluteAddress(unsigned int relativeAddress) {
    int AbsoluteAddress = (int) ((relativeAddress * 32) + p_start);
    return (void*)AbsoluteAddress;
}

//this function assume the allocated header is set correctly
void PushToBucket(UnallocatedBlock_t *pointer) {
    unsigned int size = GetSize(pointer->Header.Size);
    int bucketNum = ceil_log2(size)-1;
    if(bucketNum < 0)
        bucketNum++;
    if(size > 2*LowerLimit[bucketNum]-1)
        bucketNum++;
    if (bucketNum < 0 || bucketNum >= 11)
        return;//bucket out of range
    if(BucketArray[bucketNum] != NULL){
        BucketArray[bucketNum]->PrevFree = pointer;
    }
    pointer->NextFree = BucketArray[bucketNum];
    pointer->PrevFree = NULL;
    BucketArray[bucketNum] = pointer;
}

void RemoveBlockFromBucket(UnallocatedBlock_t *pointer) {
    unsigned int size = GetSize(pointer->Header.Size);
    int bucketNum = GetBucketNumOfAGivenSize(size);

    //assign prevfree's next free pointer
    if (BucketArray[bucketNum] == pointer) {// popping the first one on the list. there is no prev free
        BucketArray[bucketNum] = pointer->NextFree;
        if (BucketArray[bucketNum] != NULL)// only one thing in the bucket
            BucketArray[bucketNum]->PrevFree = NULL;
    }
    else{
        if(pointer->PrevFree != NULL)
            pointer->PrevFree->NextFree = pointer->NextFree;
        if(pointer->NextFree != NULL)
            pointer->NextFree->PrevFree = pointer->PrevFree;
    }
}

UnallocatedBlock_t *PopBucket(int chunks) {
		UnallocatedBlock_t *ptrToBucketNum = NULL;
    int bucketNum = ceil_log2((unsigned long long int) chunks)-1;
    if (bucketNum < 0)
        bucketNum++;
    if(chunks > LowerLimit[bucketNum])
        bucketNum++;// +1 because you take the guaranteed bucket
    if (bucketNum < 0 || bucketNum >= 11)
        return NULL;
    while (BucketArray[bucketNum] == NULL) {
        bucketNum = bucketNum + 1;
        if (bucketNum >= 11)
            return NULL;
    }
    ptrToBucketNum = BucketArray[bucketNum];
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
    header->Header.LastBlock = 1;
    header->NextFree = NULL;
    header->PrevFree = NULL;
    for (; i < 10; i++)
        BucketArray[i] = NULL;
    BucketArray[10] = header;
}


AllocatedBlock_t *splitBlock(UnallocatedBlock_t *freeBlock, int numChunk) {
    UnallocatedBlock_t* SSB = NULL;
    unsigned int sizeFreeTotal = GetSize(freeBlock->Header.Size);

    AllocatedBlock_t *allocBlock = (AllocatedBlock_t *) freeBlock;
    allocBlock->Size = (unsigned int) numChunk;
    allocBlock->Allocated = 1;

    SSB = GetAbsoluteAddress(GetRelativeAddress(freeBlock) + numChunk);

     // changing freeBlock from the original
    // sized block to the second (freeBlock) when split
    SSB->Header.Size = (unsigned int)(sizeFreeTotal - numChunk);
    SSB->Header.PrevBlock = GetRelativeAddress(allocBlock);
    SSB->Header.Allocated = 0;
    if(allocBlock->LastBlock){
        allocBlock->LastBlock = 0;
        SSB->Header.LastBlock = 1;
        SSB->Header.NextBlock = 0;
    }
    else{
        ((AllocatedBlock_t*) GetAbsoluteAddress(allocBlock->NextBlock))->PrevBlock = GetRelativeAddress(SSB);
        SSB->Header.NextBlock = allocBlock->NextBlock;
    }

    allocBlock->NextBlock = GetRelativeAddress(SSB);

    PushToBucket(SSB);
    return allocBlock;
}

/**********
size is the number of bytes that the user wants to allocate


**********/
void *half_alloc(int size) {
		UnallocatedBlock_t *freeBlock = NULL;
    if (size <= 0 || size >32764)
        return NULL;
    size += 4; // 4 bytes are required for header
    size = (size + 31) / 32; // this divide size by 32 and ceil it REWRITE IN BITWISE
    freeBlock = PopBucket(size);
    if (freeBlock == NULL)
        return NULL;
    if (freeBlock->Header.Size == size || (freeBlock->Header.Size+1024) == size) {
        freeBlock->Header.Allocated = 1;
        return (AllocatedBlock_t*)(((unsigned int)freeBlock)+4);
    }
    return (AllocatedBlock_t*)(((unsigned int)splitBlock(freeBlock, size))+4);
}


UnallocatedBlock_t* coco (UnallocatedBlock_t* first, UnallocatedBlock_t* second){
    first->Header.Size = first->Header.Size + second->Header.Size;
    first->Header.Allocated = 0;
    if(second->Header.NextBlock == GetRelativeAddress(second)){
        first->Header.NextBlock = GetRelativeAddress(first);
    }
    else{
        first->Header.NextBlock = second->Header.NextBlock;
    }
    if(second->Header.LastBlock)
        first->Header.LastBlock = 1;

    if(first->Header.PrevBlock == GetRelativeAddress(first)){
        if(GetRelativeAddress(first) != 0)
            printf ("no prev block and not first\n");
    }
    else if(first->Header.PrevBlock > GetRelativeAddress(first)){
        printf("You fked up");
    }

    return first;
}

void half_free(void *pointer) {
    UnallocatedBlock_t *CurrentBlockHeader = (UnallocatedBlock_t *) ((unsigned int)pointer - 4);
    unsigned int size = GetSize(CurrentBlockHeader->Header.Size);
	  UnallocatedBlock_t *FirstCoalesceBlock = CurrentBlockHeader;
    UnallocatedBlock_t *LastCoalesceBlock = CurrentBlockHeader;
    UnallocatedBlock_t *PrevBlock = (UnallocatedBlock_t *) GetAbsoluteAddress(CurrentBlockHeader->Header.PrevBlock);
    UnallocatedBlock_t *NextBlock = (UnallocatedBlock_t *) GetAbsoluteAddress(CurrentBlockHeader->Header.NextBlock);
    UnallocatedBlock_t *NextBlockAfterCoalesce = NULL;

    if(pointer == NULL)
        return;
    if(!CurrentBlockHeader->Header.Allocated)
        return;

//    if(CurrentBlockHeader->Header.PrevBlock != GetRelativeAddress(CurrentBlockHeader) &&
//       ((AllocatedBlock_t*)GetAbsoluteAddress(CurrentBlockHeader->Header.PrevBlock))->Allocated ==0){
//        RemoveBlockFromBucket(GetAbsoluteAddress(CurrentBlockHeader->Header.PrevBlock));
//        CurrentBlockHeader = coco(GetAbsoluteAddress(CurrentBlockHeader->Header.PrevBlock),CurrentBlockHeader);
//    }
//    if(CurrentBlockHeader->Header.NextBlock != GetRelativeAddress(CurrentBlockHeader)&&
//       ((AllocatedBlock_t*)GetAbsoluteAddress(CurrentBlockHeader->Header.NextBlock))->Allocated ==0){
//        RemoveBlockFromBucket(GetAbsoluteAddress(CurrentBlockHeader->Header.NextBlock));
//        CurrentBlockHeader = coco(CurrentBlockHeader, GetAbsoluteAddress(CurrentBlockHeader->Header.NextBlock));
//    }
//    PushToBucket(CurrentBlockHeader);

    if ((unsigned int)CurrentBlockHeader != p_start && PrevBlock != CurrentBlockHeader && PrevBlock->Header.Allocated==0) {
        FirstCoalesceBlock = PrevBlock;
        size += PrevBlock->Header.Size;
        RemoveBlockFromBucket(PrevBlock);
    }
    if (NextBlock->Header.Allocated == 0 && (unsigned int)NextBlock != p_start) {
        LastCoalesceBlock = NextBlock;
        size += NextBlock->Header.Size;
        RemoveBlockFromBucket(NextBlock);
    }

    //Prev block should be already set
    FirstCoalesceBlock->Header.Allocated = 0;
    FirstCoalesceBlock->Header.Size = (unsigned int) size;

    FirstCoalesceBlock->Header.NextBlock = LastCoalesceBlock->Header.NextBlock;
    if (LastCoalesceBlock->Header.LastBlock) {//there is no nextblock
        FirstCoalesceBlock->Header.NextBlock = 0;
        FirstCoalesceBlock->Header.LastBlock = 1;
    }
    else {
        NextBlockAfterCoalesce = GetAbsoluteAddress(FirstCoalesceBlock->Header.NextBlock);
        NextBlockAfterCoalesce->Header.PrevBlock = GetRelativeAddress(FirstCoalesceBlock);
    }

    PushToBucket(FirstCoalesceBlock);
}
