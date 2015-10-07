#ifndef __BST_H__
#define __BST_H__

/*
 * Author names:
 *   1.  uWaterloo User ID:  w52jiang@uwaterloo.ca
 *   2.  uWaterloo User ID:  pshering@uwaterloo.ca
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "type.h"

typedef struct bsn{
	S32 val;
	struct bsn *left, *right;
}  bsn_t;

typedef struct bst{
	bsn_t  *root;
	size_t size;
} bst_t;

void bst_init( bst_t * );
void bst_destroy( bst_t * );

size_t bst_size( bst_t * );

bool bst_insert( bst_t *, S32 );
bool bst_erase( bst_t *, S32 );

S32 bst_min( bst_t * );
S32 bst_max( bst_t * );

#endif
