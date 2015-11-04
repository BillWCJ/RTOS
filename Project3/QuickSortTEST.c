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
          arr[i]=member;
          break;
       }
    }
}

void insertsort(int arr[],int size)
{
    int newsize=1,member;
    for(newsize=1;newsize<size;newsize++){
        member=arr[newsize];
        insert(member,arr,newsize);
    }
}



void quicksort0(int arr[], int a, int b) {
    if (a >= b)
        return;

    int key = arr[a];
    int i = a + 1, j = b;
    while (i < j) {
        while (i < j && arr[j] >= key)
            --j;
        while (i < j && arr[i] <= key)
            ++i;
        if (i < j)
            swap(arr, i, j);
    }
    if (arr[a] > arr[i]) {
        swap(arr, a, i);
        quicksort0(arr, a, i - 1);
        quicksort0(arr, i + 1, b);
    } else { // there is no left-hand-side
        quicksort0(arr, a + 1, b);
    }
}

int main (){
  int testarray[20] = {11,1,2,13,20,5,5,87,0,15,21,27,12,3,14,15,6,17,18,19};
  // quicksort0(testarray, 0, 20);
  insertsort(testarray, 20);
  for(int i = 0; i < 20; i++) {
        printf("%d ", testarray[i]);
    }

  return 0;

}
