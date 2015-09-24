#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "bst.h"

void bst_init( bst_t *tree ) {
    //initalize all values to default
    tree -> root = NULL;
    tree -> size = 0;
}

void bst_destroy( bst_t *tree ) {
    while(bst_size(tree) > 0){
        //keep on deleting the min value until empty
        bst_erase(tree, bst_min(tree));
	}
}

size_t bst_size( bst_t *tree ) {
	return tree -> size;
}

BOOL bst_insert( bst_t *tree, S32 val ) {
    //____FALSE
	if (bst_exist(tree, val))
		return __FALSE;

	if (tree -> root == NULL)
	{
        tree -> root = new_node(val);
        tree -> size++;
    	return __TRUE;
	}

    bsn_t* visitor = tree -> root;
    while(visitor -> val != val)
	{
		if(visitor -> val > val)
        {
            if( visitor -> left == NULL)
            {
                visitor -> left = new_node(val);
                tree -> size++;
                return __TRUE;
            }
            visitor = visitor -> left;
        }
        else
        {
            if(visitor -> right == NULL)
            {
                visitor -> right = new_node(val);
                tree -> size++;
                return __TRUE;
            }
            visitor = visitor -> right;
        }
    }
    return __FALSE;
}

S32 bst_min( bst_t *tree ) {
    bsn_t* visitor = tree -> root;
	while (visitor -> left != NULL)
	{
		visitor = visitor -> left;
	}
	return visitor -> val;
}

S32 bst_max( bst_t *tree ) {
    bsn_t* visitor = tree -> root;
	while (visitor -> right != NULL)
	{
		visitor = visitor -> right;
	}
	return visitor -> val;
}

BOOL bst_erase( bst_t *tree, S32 val ) {
    if (!bst_exist(tree, val))
    	return __FALSE;

    else
    {
		bsn_t* currentNode = tree -> root;
		bsn_t* parentNode = NULL;

		if (tree -> size == 1)
		{
			free(currentNode);
			tree -> root = NULL;
			tree -> size--;
			return __TRUE;
		}
		else if(currentNode -> val == val && (currentNode -> right == NULL || currentNode -> left == NULL))
		{
			if (currentNode -> right==NULL)
			{
				tree -> root = currentNode -> left;
				free(currentNode);
			}
			else
			{
				tree -> root = currentNode -> right;
				free(currentNode);
			}
			tree -> size--;
			return __TRUE;
		}

		while(currentNode -> val != val)
		{
			if(val > currentNode -> val)
			{
				parentNode = currentNode;
				currentNode = currentNode -> right;
			}
			else
			{
				parentNode = currentNode;
				currentNode = currentNode -> left;
			}
		}

		if(currentNode -> right == NULL && currentNode -> left == NULL)
		{
			if(parentNode -> right == currentNode)
			{
				parentNode -> right = NULL;
				free(currentNode);
			}
			else
			{
				parentNode -> left = NULL;
				free(currentNode);
			}
		}
		else if(currentNode -> right != NULL && currentNode->left != NULL)
		{
			bsn_t* maxNode = currentNode -> left;
			bsn_t* parmaxNode = currentNode;

			while (maxNode -> right != NULL)
			{
				parmaxNode = maxNode;
				maxNode = maxNode -> right;
			}
			currentNode -> val = maxNode -> val;
			if (currentNode == parmaxNode)
			{
				if(maxNode -> left != NULL)
				{
					parmaxNode -> left = maxNode -> left;
				}
				else
				{
					parmaxNode -> left = NULL;
				}
			}
			else
			{
				if(maxNode -> left != NULL)
				{
					parmaxNode -> right = maxNode -> left;
				}
				else
				{
					parmaxNode -> right = NULL;
				}
			}

			free(maxNode);
			//print();
		}
		else
		{
			if(currentNode -> right == NULL)
			{
				if(parentNode -> right == currentNode)
				{
					parentNode -> right = currentNode -> left;
					free(currentNode);
				}
				else
				{
					parentNode -> left = currentNode -> left;
					free(currentNode);
				}
			}

			if(currentNode -> left == NULL)
			{
				if(parentNode -> right == currentNode)
				{
					parentNode -> right = currentNode -> right;
					free(currentNode);
				}
				else
				{
					parentNode -> left = currentNode -> right;
					free(currentNode);
				}
			}
		}
    	tree -> size--;
    	return __TRUE;
    }
}

BOOL bst_exist( bst_t *tree, S32 val ) {
    if(tree -> root == NULL)
        return __FALSE;

    bsn_t* visitor = tree -> root;
	while (visitor != NULL)
	{
		if (visitor -> val == val)
            return __TRUE;
        else if (val < visitor -> val)
            visitor = visitor -> left;
        else
            visitor = visitor -> right;
    }
    return __FALSE;
}

bst_t* new_node(S32 val){
    bsn_t* newNode = (bsn_t*)malloc(sizeof(bsn_t));
    newNode -> left = NULL;
    newNode -> right = NULL;
    newNode -> val = val;
    return newNode;
}
