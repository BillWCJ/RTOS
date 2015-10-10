#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "half_fit.h"

UnallocatedBlock_t *BucketArray[11] = {0,0,0,0,0,0,0,0,0,0,0};
const int LowerLimit[11] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
static char array[ARR_SZ];
//The base address. On keil this would be 0x10000000, but on our computer we have to use a dynamic one
void *p_start = array;

void check(void){
    int numblock = 0;
    bool lala = true;
    AllocatedBlock_t* temp = p_start;
    while(temp <= (p_start+(1024*32))){
        numblock++;
        if(temp->NextBlock == 0 && !lala)
            break;
        temp = GetAbsoluteAddress(temp->NextBlock);
    }
    int num2 = CheckPrev(temp);
    if(numblock != num2)
        printf("-----------%d != %d\n", numblock, num2);
}

void PrintPrev(AllocatedBlock_t* pointer){
    if(pointer == p_start){
        if(pointer->Allocated==0)
            printf("%d!",GetRelativeAddress(pointer));
        else
            printf("%d.",GetRelativeAddress(pointer));
    }
    else{
        PrintPrev(GetAbsoluteAddress(pointer->PrevBlock));
        if(pointer->Allocated==0)
            printf("%d!",GetRelativeAddress(pointer));
        else
            printf("%d.",GetRelativeAddress(pointer));
    }
}

int CheckPrev(AllocatedBlock_t* pointer){
    if(pointer == p_start){
            return 1;
    }
    else{
        return CheckPrev(GetAbsoluteAddress(pointer->PrevBlock)) + 1;
    }
}
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
    int i = 0;
    AllocatedBlock_t* temp = p_start;
    bool lala = true;
    bool startPrint = false;
    while(temp <= (p_start+(1024*32))){
        int location = GetRelativeAddress(temp);
        if(temp->Allocated==0)
            printf("%d!",GetRelativeAddress(temp));
        else
            printf("%d.",GetRelativeAddress(temp));

        if(temp->NextBlock == 0 && !lala)
            break;
        temp = GetAbsoluteAddress(temp->NextBlock);

        lala = false;
    }
    printf("\n");
    PrintPrev(temp);
    printf("\n");
    for (; i < 11; i++){
       if(BucketArray[i] ==NULL)
            printf("a%d:XXXXX",i);
       else
            printf("a%d:%5d",i,(((int)BucketArray[i]-(int)p_start)+31)/32);
    }
    printf("\n");
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

unsigned int inline GetRelativeAddress(void *pointer) {
    unsigned int relativeAddress = (unsigned int) (((unsigned int)pointer - (unsigned int)p_start) / 32);
    return relativeAddress;
}


inline void *GetAbsoluteAddress(unsigned int relativeAddress) {
    int AbsoluteAddress = (int) ((relativeAddress * 32) + p_start);
    return AbsoluteAddress;
}

//this function assume the allocated header is set correctly
void inline PushToBucket(UnallocatedBlock_t *pointer) {
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

inline UnallocatedBlock_t *PopBucket(int chunks) {
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
    header->Header.LastBlock = 1;
    header->NextFree = NULL;
    header->PrevFree = NULL;
    for (; i < 10; i++)
        BucketArray[i] = NULL;
    BucketArray[10] = header;
}


AllocatedBlock_t inline *splitBlock(UnallocatedBlock_t *freeBlock, int numChunk) {
    UnallocatedBlock_t* SSB = NULL;
    unsigned int sizeFreeTotal = GetSize(freeBlock->Header.Size);

    AllocatedBlock_t *allocBlock = (AllocatedBlock_t *) freeBlock;
    allocBlock->Size = (unsigned int) numChunk;
    allocBlock->Allocated = 1;

    unsigned int blah = GetRelativeAddress(freeBlock);
    SSB = GetAbsoluteAddress(blah + numChunk);

     // changing freeBlock from the original
    // sized block to the second (freeBlock) when split
    unsigned int size2 = (unsigned int)(sizeFreeTotal - numChunk);
    SSB->Header.Size = size2;
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
    if (size <= 0 || size >32764)
        return NULL;
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
    if(pointer == NULL)
        return;
    UnallocatedBlock_t *CurrentBlockHeader = (UnallocatedBlock_t *) (pointer - 4);
    if(!CurrentBlockHeader->Header.Allocated)
        return;
    unsigned int size = GetSize(CurrentBlockHeader->Header.Size);

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

    UnallocatedBlock_t *FirstCoalesceBlock = CurrentBlockHeader;
    UnallocatedBlock_t *LastCoalesceBlock = CurrentBlockHeader;
    UnallocatedBlock_t *PrevBlock = (UnallocatedBlock_t *) GetAbsoluteAddress(CurrentBlockHeader->Header.PrevBlock);
    UnallocatedBlock_t *NextBlock = (UnallocatedBlock_t *) GetAbsoluteAddress(CurrentBlockHeader->Header.NextBlock);
    UnallocatedBlock_t *NextBlockAfterCoalesce = NULL;

    if (CurrentBlockHeader != p_start && PrevBlock != CurrentBlockHeader && PrevBlock->Header.Allocated==0) {
        FirstCoalesceBlock = PrevBlock;
        size += PrevBlock->Header.Size;
        RemoveBlockFromBucket(PrevBlock);
    }
    bool isLastBlock = CurrentBlockHeader->Header.LastBlock == 0;
    if (NextBlock->Header.Allocated==0 && NextBlock != p_start) {
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
