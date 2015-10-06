#ifndef HALF_FIT_H_
#define HALF_FIT_H_

#define ARR_SZ 32768

/*
 * Author names:
 *   1.  uWaterloo User ID:  w52jiang@uwaterloo.ca
 *   2.  uWaterloo User ID:  pshering@uwaterloo.ca
 */

typedef struct AllocatedBlock {
    unsigned int PrevBlock : 10;
    unsigned int NextBlock : 10;
    unsigned int Size : 10;
    unsigned int Allocated : 1;
} AllocatedBlock_t;
//Method to get and set all the properties 8X

typedef struct UnallocatedBlock{
    AllocatedBlock_t Header;
    unsigned int PrevFree : 10;
    unsigned int NextFree : 10;
} UnallocatedBlock_t;


void  half_init( void );
void *half_alloc( int );
// or void *half_alloc( unsigned int );
void  half_free( void * );

#endif
