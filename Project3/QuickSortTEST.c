#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#include "quicksort.h"
#include "array_tools.h"

void swap(int arr[], int i, int j) {
    int temp = arr[j];
    arr[j] = arr[i];
    arr[i] = temp;
}
void insert(int member,int arr[],int size)
{
    int i,j;
    for(i=0;i<size;i++)
    {
      if(member<arr[i])
      {
          for( j=0;j<size-i;j++)
          {
              arr[size-j]=arr[size-j-1];
          }
          arr[i] = member;
          break;
       }
    }
}

void insertSort(int arr[],int size)
{
    int newsize=1,member;
    for(newsize=1;newsize<size;newsize++){
        member=arr[newsize];
        insert(member,arr,newsize);
    }
}



void quickSort(int arr[], int a, int b) {
    if (a >= b)
        return;

    int key = arr[a];
    int i = a + 1, j = b;
    while (i < j)  {
        while (i < j && arr[j] >= key)
            --j;
        while (i < j && arr[i] <= key)
            ++i;
        if (i < j)
            swap(arr, i, j);
    }
    if (arr[a] > arr[i]) {
        swap(arr, a, i);
        quickSort(arr, a, i - 1);
        quickSort(arr, i + 1, b);
    } else { // if there is no left-hand-side
        quickSort(arr, a + 1, b);
    }
}

// bool is_sorted_array( int array, int length ) {
// 	int i;
//
// 	for ( i = 1; i < length; ++i ) {
// 		if ( array[i - 1] > array[i] ) {
// 			printf("The array is not sorted in [%d]=%d > [%d]=%d\n", i-1, array[i-1], i, array[i]);
// 			return false;
// 		}
// 	}
//
// 	return true;
// }

int main (){
    bool sorted = false;
    int testarray[25] = {11,1,2,13,20,5,5,87,0,15,21,27,12,3,14,15,6,17,18,19, 35, 82, 2, 45, 10};
    for(int i = 0; i < 25; i++) {
        printf("%d ", testarray[i]);
    }
    printf("\n" );
    // quickSort(testarray, 0, 25);
    insertSort(testarray, 25);
    for(int i = 0; i < 25; i++) {
        printf("%d ", testarray[i]);
    }
    printf("\n" );

    // array_t test;
    // tes
    // bool sorted = is_sorted_array(testarray, 25);
    // printf("%i\n", sorted );

    return 0;
}
