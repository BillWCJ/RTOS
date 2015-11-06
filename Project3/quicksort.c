// #include <LPC17xx.h>
// #include <RTL.h>
// #include <stdbool.h>
// #include <stdio.h>
// #include <stdlib.h>

// #include "quicksort.h"
// #include "array_tools.h"

// // You decide what the threshold will be
// #define USE_INSERTION_SORT 30

// // Local prototypes
// void generate_quickSort(array_t array, size_t start, size_t end, unsigned char pri);


// size_t use_insert = 5;
// volatile int counter;
// OS_SEM sem;

// //structs
// typedef struct {
// 	array_t array;
// 	size_t start;
// 	size_t end;
// } array_interval_t;

// typedef struct{
// 	array_interval_t interval;
// 	unsigned char priority;
// } qsort_task_parameters_t;

// void increase_counter(bool inc){
// 	printf("{");
// 	os_sem_wait(&sem,0xffff);{
// 		if(inc)
// 			counter++;
// 		else
// 			counter--;
// 	}os_sem_send(&sem);
// 	printf("%i}", counter);
// }

// //functions
// array_type median_of_three(array_type arr[], size_t lo, size_t hi){
// 		size_t middle_index = lo + (hi - lo)/2;   // BIT SHIFT HERE
//     array_type a = arr[lo];
//     array_type b = arr[middle_index];
//     array_type c = arr[hi];
// 		
//     if (a<b) {
//        // partial order = a,b
//         if (b<c) {
//             // 2 comparisons: order is a,b,c
//             return middle_index;
//         }
//         else { // order is a,c,b or c,a,b
//             if (a<c) {
//                 // order is a,c,b -- 3 comparisons
//                 return hi;
//             }
//             else {
//                 // order is c,a,b -- 3 comparisons
//                 return lo;
//             }
//         }
//     } else {
//         // partial order = b,a
//         if (c<b) {
//             // 2 comparisons: order is c,b,a
//             return middle_index;
//         }
//         else {  // order is b,c,a or b,a,c
//             if (c>a) {
//                 // order is b,a,c -- 3 comparisons
//                 return lo;
//             }
//             else {
//                 // order is b,c,a -- 3 comparisons
//                 return hi;
//             }
//         }
//     }
// }
// /// ###################### WORKING QUICK SORT
// // void swap(array_type arr[], size_t i, size_t j) {
// //     int temp = arr[j];
// //     arr[j] = arr[i];
// //     arr[i] = temp;
// // }
// // void quickSort(array_type arr[], size_t lo, size_t hi) {
// //    size_t key = 0;
// // 		size_t i = 0, j = 0;
// // 		if (lo >= hi)
// //         return;
// //     key = arr[lo];
// //     i = lo + 1;
// // 		j = hi;
// //     while (i < j) {
// //         while (i < j && arr[j] >= key)
// //             --j;
// //         while (i < j && arr[i] <= key)
// //             ++i;
// //         if (i < j)
// //             swap(arr, i, j);
// //     }
// //     if (arr[lo] > arr[i]) {
// //         swap(arr, lo, i);
// //         quickSort(arr, lo, i - 1);
// //         quickSort(arr, i + 1, hi);
// //     } else { // there is no left-hand-side
// //         quickSort(arr, lo + 1, hi);
// //     }
// // }
// // // #################################

// void swap(array_type arr[], size_t i, size_t j) {
//     size_t temp = arr[j];
//     arr[j] = arr[i];
//     arr[i] = temp;
// }
// void insert(size_t member, array_type arr[],size_t size)
// {
//     size_t i,j;
//     for(i=0;i<size;i++)
//     {
//       if(member<arr[i])
//       {
//           for( j=0;j<size-i;j++)
//           {
//               arr[size-j]=arr[size-j-1];
//           }
//           arr[i] = member;
//           break;
//        }
//     }
// }

// void insertSort(array_type arr[], size_t size)
// {
//     size_t newsize=1, member;
//     for(newsize=1;newsize<size;newsize++){
//         member=arr[newsize];
//         insert(member,arr,newsize);
//     }
// }

// // void quickSort(array_type arr[], size_t lo, size_t hi) {
// // 		size_t key = 0;
// // 		size_t i = 0, j = 0;
// //     if (lo >= hi)
// //         return;

// //     key = arr[lo];
// //     i = lo + 1;
// // 		j = hi;
// //     while (i < j) {
// //         while (i < j && arr[j] >= key)
// //             --j;
// //         while (i < j && arr[i] <= key)
// //             ++i;
// //         if (i < j)
// //             swap(arr, i, j);
// //     }
// //     if (arr[lo] > arr[i]) {
// //         swap(arr, lo, i);
// //         quickSort(arr, lo, i - 1);
// //         quickSort(arr, i + 1, hi);
// //     } else { // if there is no left-hand-side
// //         quickSort(arr, lo + 1, hi);
// //     }
// // }

// __task void quick_sort_task( void* void_ptr){
// 		qsort_task_parameters_t *task_param = (qsort_task_parameters_t*)void_ptr;
// 		size_t lo = task_param->interval.start;
// 		size_t hi = task_param->interval.end;
// 		size_t length = (task_param->interval.end - task_param->interval.start + 1);
// 		size_t key = 0;
// 		size_t i = 0, j = 0;
// 		array_type *arr = task_param->interval.array.array;
// 	
// 		if ((length > USE_INSERTION_SORT) && (task_param->priority > 36)){ // || or # threads == 31 in other words priorities == 31
// 				// QUICK SORT
// 				if (lo >= hi)
// 						return;

// 				key = arr[lo];
// 				i = lo + 1;
// 				j = hi;
// 				while (i < j) {
// 						while (i < j && arr[j] >= key)
// 								--j;
// 						while (i < j && arr[i] <= key)
// 								++i;
// 						if (i < j)
// 								swap(arr, i, j);
// 				}
// 				if (arr[lo] > arr[i]) {
// 						swap(arr, lo, i);
// 						generate_quickSort(task_param->interval.array, lo, i - 1, task_param->priority);
// 						generate_quickSort(task_param->interval.array, i + 1, hi, task_param->priority);
// 				} 
// 				else { // if there is no left-hand-side
// 						generate_quickSort(task_param->interval.array, lo + 1, hi, task_param->priority);
// 				}
// 		}				//END OF QUICK SORT
// 		else 
// 				insertSort(task_param->interval.array.array, length );
// 		os_tsk_delete_self();
// }

// void generate_quickSort(array_t array, size_t start, size_t end, unsigned char pri){
// 		array_interval_t interval;
// 		qsort_task_parameters_t task_param;

// 		// Based on MTE 241 course notes--you can change this if you want
// 		//  - in the course notes, this sorts from a to c - 1
// 		interval.array =  array;
// 		interval.start     =  start;
// 		interval.end     =  end;

// 		task_param.interval = interval;

// 		// If you are using priorities, you can change this
// 		task_param.priority = pri+1;
// 		
// 		os_tsk_create_ex( quick_sort_task, task_param.priority, &task_param );
// }

// void quicksort( array_t array ) {
// 	array_interval_t interval;
// 	qsort_task_parameters_t task_param;
// 	OS_RESULT result = OS_R_OK;

// 	// Based on MTE 241 course notes--you can change this if you want
// 	//  - in the course notes, this sorts from a to c - 1
// 	interval.array =  array;
// 	interval.start =  0;
// 	interval.end =  array.length - 1;

// 	task_param.interval = interval;

// 	// If you are using priorities, you can change this
// 	task_param.priority = 5;
// 	os_sem_init (&sem, 0);
// 	
// 	//start the quick_sort threading
// 	printf("before");
// 	os_tsk_create_ex( quick_sort_task, task_param.priority, &task_param );
// 	printf("after");

// //			os_dly_wait(5);
// // 	while(1){
// // 		result = os_sem_wait(&sem,1);
// // 		if(result == OS_R_OK){
// // 			os_sem_send(&sem);
// // 			os_dly_wait(1);
// // 			printf("gg");
// // 		}
// // 		else if(result == OS_R_TMO)
// // 			break;
// // 	}
// }
// // #include <LPC17xx.h>
// // #include <RTL.h>
// // #include <stdbool.h>
// // #include <stdio.h>
// // #include <stdlib.h>

// // #include "quicksort.h"
// // #include "array_tools.h"

// // // You decide what the threshold will be
// // #define USE_INSERTION_SORT 5

// // // Local prototypes
// // void generate_quickSort(array_t array, size_t start, size_t end, unsigned char pri);

// // const bool usesem = false;
// // OS_SEM sem;

// // //structs
// // typedef struct {
// // 	array_t array;
// // 	size_t start;
// // 	size_t end;
// // } array_interval_t;

// // typedef struct{
// // 	array_interval_t interval;
// // 	unsigned char priority;
// // } qsort_task_parameters_t;

// // void swap(array_type arr[], size_t i, size_t j) {
// //     size_t temp = arr[j];
// //     arr[j] = arr[i];
// //     arr[i] = temp;
// // }

// // size_t median_of_three(array_type arr[], size_t lo, size_t hi){
// //     size_t middle_index = (lo + hi)/2;
// //     array_type a = arr[lo];
// //     array_type b = arr[middle_index];
// //     array_type c = arr[hi];

// //     if (a<b) {
// //        // partial order = a,b
// //         if (b<c) {
// //             // 2 comparisons: order is a,b,c
// //             return middle_index;
// //         }
// //         else { // order is a,c,b or c,a,b
// //             if (a<c) {
// //                 // order is a,c,b -- 3 comparisons
// //                 return hi;
// //             }
// //             else {
// //                 // order is c,a,b -- 3 comparisons
// //                 return lo;
// //             }
// //         }
// //     } else {
// //         // partial order = b,a
// //         if (c<b) {
// //             // 2 comparisons: order is c,b,a
// //             return middle_index;
// //         }
// //         else {  // order is b,c,a or b,a,c
// //             if (c>a) {
// //                 // order is b,a,c -- 3 comparisons
// //                 return lo;
// //             }
// //             else {
// //                 // order is b,c,a -- 3 comparisons
// //                 return hi;
// //             }
// //         }
// //     }
// // }

// // void insertion_sort(array_type* arr, size_t start, size_t end) {
// //     size_t j = 0;
// //     size_t i = start;
// //     array_type temp;

// //     for (;i <= end; i++){
// // 		j = i;
// // 		while (j > start && arr[j] < arr[j-1]){
// //             temp = arr[j];
// //             arr[j] = arr[j-1];
// //             arr[j-1] = temp;
// //             j--;
// //         }
// //     }
// // }
// // // void insert(size_t member, array_type arr[],size_t size)
// // // {
// // //     size_t i,j;
// // //     for(i=0;i<size;i++)
// // //     {
// // //       if(member<arr[i])
// // //       {
// // //           for( j=0;j<size-i;j++)
// // //           {
// // //               arr[size-j]=arr[size-j-1];
// // //           }
// // //           arr[i] = member;
// // //           break;
// // //        }
// // //     }
// // // }

// // // void insertion_sort(array_type* arr, size_t start, size_t end)
// // // {
// // //     size_t newsize=1, member;
// // //     for(newsize=1;newsize< (end - start);newsize++){
// // //         member=arr[newsize];
// // //         insert(member,arr,newsize);
// // //     }
// // // }
// // __task void quick_sort_task( void* void_ptr){
// //       qsort_task_parameters_t *task_param = (qsort_task_parameters_t*)void_ptr;
// //       size_t lo =task_param->interval.start;
// //       size_t hi = task_param->interval.end;

// // 			size_t key = 0;
// // 			size_t i = 0, j = 0;
// //       array_type* arr = task_param->interval.array.array;

// //     if (lo >= hi)
// //         return;
// // 		if(usesem){
// // 			os_sem_send(&sem);
// // 			printf("<");
// // 		}
// // 		if(hi - lo > USE_INSERTION_SORT){
// // 			if (lo >= hi)
// // 					return;

// // 			key = arr[lo];
// // 			i = lo + 1;
// // 			j = hi;
// // 			while (i < j) {
// // 					while (i < j && arr[j] >= key)
// // 							--j;
// // 					while (i < j && arr[i] <= key)
// // 							++i;
// // 					if (i < j)
// // 							swap(arr, i, j);
// // 			}
// // 			if (arr[lo] > arr[i]) {
// // 					swap(arr, lo, i);
// // 					generate_quickSort(task_param->interval.array, lo, i - 1,task_param->priority);
// // 					generate_quickSort(task_param->interval.array, i + 1, hi,task_param->priority);
// // 			} else { // if there is no left-hand-side
// // 					generate_quickSort(task_param->interval.array, lo + 1, hi,task_param->priority);
// // 			}

// // 		}
// // 		else
// // 			insertion_sort(arr, lo, hi);
// // 		
// // 		if(usesem){
// // 			os_sem_wait(&sem, 0xFFFF);
// // 			printf(">");
// // 		}
// // 		os_tsk_delete_self();
// // }

// // void generate_quickSort(array_t array, size_t start, size_t end, unsigned char pri){
// // 	array_interval_t interval;
// // 	qsort_task_parameters_t task_param;

// // 	// Based on MTE 241 course notes--you can change this if you want
// // 	//  - in the course notes, this sorts from a to c - 1
// // 	interval.array =  array;
// // 	interval.start     =  start;
// // 	interval.end     =  end;

// // 	task_param.interval = interval;

// // 	// If you are using priorities, you can change this
// // 	task_param.priority = usesem ? pri : pri+1;
// // 	os_tsk_create_ex( quick_sort_task, task_param.priority, &task_param );
// // }

// // void quicksort( array_t array ) {
// // 	array_interval_t interval;
// // 	qsort_task_parameters_t task_param;
// // 	OS_RESULT result = OS_R_OK;
// // 	
// // 	// Based on MTE 241 course notes--you can change this if you want
// // 	//  - in the course notes, this sorts from a to c - 1
// // 	interval.array =  array;
// // 	interval.start     =  0;
// // 	interval.end     =  array.length - 1;

// // 	task_param.interval = interval;

// // 	// If you are using priorities, you can change this
// // 	task_param.priority = 1;
// // 	os_sem_init(&sem, 0);
// // 	
// // 	//start the quick_sort threading
// // 	os_tsk_create_ex( quick_sort_task, task_param.priority, &task_param );
// // 	while(1){
// // 		result = os_sem_wait(&sem,1);
// // 		if(result == OS_R_OK){
// // 			os_sem_send(&sem);
// // 			os_dly_wait(1);
// // 			printf("gg");
// // 		}
// // 		else if(result == OS_R_TMO)
// // 			break;
// // 	}
// // 	printf("fd");
// // }

#include <LPC17xx.h>
#include <RTL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "quicksort.h"
#include "array_tools.h"

bool usesem = true;
bool usequick = true;
size_t useInsert=0;
//OS_SEM mainLock;
OS_MUT mut;
// int mainLock;
// int mut;
int counter;

//structs
typedef struct {
	array_t array;
	size_t start;
	size_t end;
} array_interval_t;

typedef struct{
	array_interval_t interval;
	unsigned char priority;
	OS_SEM childlock;
} qsort_task_parameters_t;

// Local prototypes
OS_SEM* generate_quickSort_sem(qsort_task_parameters_t* a, array_t array, size_t start, size_t end, unsigned char priority);
void generate_quickSort(array_t array, size_t start, size_t end, unsigned char priority);

// //useless comment it out
// void os_mut_init(int* a){ *a = 1;}
// void os_mut_wait(int* a){ while(*a != 1); *a = 0;}
// void os_mut_release(int* a){ *a = 1;}
// void os_sem_init(int* a, int num){ *a =num;}
// void os_sem_wait(int* a, int count){ while(*a < 0 && count > 0) if(count !=0xffff) count--; *a = 0;}
// void os_sem_send(int* a){ (*a)++;}

void increase_counter(bool inc){
	printf("{");
	os_mut_wait(&mut, 0xffff);
	//-----Critical Region
	if(inc)
		counter++;
	else
		counter--;
	
	//unlock main and let it exit
// 	if(counter == 0)
// 		os_sem_send(&mainLock);

	//check if there are too many task
	if(counter >= 31)
		printf("There are too many task\n");
	//-----Critical Region End
	os_mut_release(&mut);
	printf("%i}", counter);
}

void print(array_type* arr, size_t lo, size_t hi){

	os_mut_wait(&mut, 0xffff);
	printf("\"%i, %i\"", lo, hi);

	os_mut_release(&mut);
}

void swap(array_type* i, array_type* j) {
  array_type temp = *j;
  *j = *i;
	*i = temp;
}

size_t median_of_three(array_type arr[], size_t lo, size_t hi){
	size_t middle_index = (lo + hi)/2;
	array_type a = arr[lo];
	array_type b = arr[middle_index];
	array_type c = arr[hi];

	if (a<b) {
	 // partial order = a,b
		if (b<c) {
			// 2 comparisons: order is a,b,c
			return middle_index;
		}
		else { // order is a,c,b or c,a,b
			if (a<c) {
				// order is a,c,b -- 3 comparisons
				return hi;
			}
			else {
				// order is c,a,b -- 3 comparisons
				return lo;
			}
		}
	} else {
		// partial order = b,a
		if (c<b) {
			// 2 comparisons: order is c,b,a
			return middle_index;
		}
		else {  // order is b,c,a or b,a,c
			if (c>a) {
				// order is b,a,c -- 3 comparisons
				return lo;
			}
			else {
				// order is b,c,a -- 3 comparisons
				return hi;
			}
		}
	}
}

void insertion_sort(array_type* arr, size_t start, size_t end) {
	size_t j = 0;
	size_t i = start+1;

	for (; i <= end; i++){
		j = i;
		while(j > start && arr[j-1] > arr[j]){
			swap(&arr[j], &arr[j-1]);
			j--;
		}
	}
}

size_t partition(array_type* arr, size_t i, size_t j){
	array_type pivot = arr[median_of_three(arr, i ,j)];
	i--;
	j++;

	//printf("(");
	while (true) {
		while (arr[++i] < pivot);
		while (arr[--j] > pivot);
		if (i < j)
			swap(&arr[i], &arr[j]);
		else
			break;
	};
	//printf(")");
	return j;
}

__task void quick_sort_task( void* void_ptr) {
	qsort_task_parameters_t *task_param = (qsort_task_parameters_t*)void_ptr;
	size_t lo =task_param->interval.start;
	size_t hi = task_param->interval.end;
	size_t pivot;

	if (lo < hi){
		if(usequick && lo+useInsert < hi){
			//less than use insertion sort limit. use quick sort still
			pivot = partition(task_param->interval.array.array, lo, hi);
			generate_quickSort(task_param->interval.array, lo, pivot, task_param->priority);
			generate_quickSort(task_param->interval.array, pivot + 1, hi, task_param->priority);
		}
		else{
			//use insertion sort
			insertion_sort(task_param->interval.array.array, lo, hi);
		}
	}
	os_tsk_delete_self();
}

void generate_quickSort(array_t array, size_t start, size_t end, unsigned char priority){
	array_interval_t interval;
	qsort_task_parameters_t task_param;

	interval.array = array;
	interval.start = start;
	interval.end = end;
	task_param.interval = interval;
	task_param.priority = priority+1;
	
	os_tsk_create_ex(quick_sort_task, task_param.priority, &task_param );
}

void quicksort( array_t array ) {
	useInsert = array.length / 80;
	if(useInsert < 5) useInsert = 5;
	
	counter = 0;
	generate_quickSort(array, 0, array.length - 1, 10);
}

__task void quick_sort_task_sem( void* void_ptr) {
	qsort_task_parameters_t *task_param = (qsort_task_parameters_t*)void_ptr;
	size_t lo =task_param->interval.start;
	size_t hi = task_param->interval.end;
	size_t pivot;

	//printf("<");
	//increase_counter(true);

	if (lo < hi){
		//printf("[");
		if(usequick && lo+useInsert < hi){
			//less than use insertion sort limit. use quick sort still
			//print(task_param->interval.array.array, lo, hi);
			pivot = partition(task_param->interval.array.array, lo, hi);
		
			if(lo < pivot){
				qsort_task_parameters_t l_child_params;
				l_child_params.interval.array = task_param->interval.array; 			
				l_child_params.interval.start = lo; 			
				l_child_params.interval.end = pivot; 			
				l_child_params.priority = task_param->priority; 			
				os_sem_init(&l_child_params.childlock, 0); 			
				os_tsk_create_ex( quick_sort_task_sem, l_child_params.priority, &l_child_params ); 			
				os_sem_wait(&l_child_params.childlock, 0xffff);
			}
			if(pivot +1 < hi){
				qsort_task_parameters_t r_child_params;
				r_child_params.interval.array = task_param->interval.array; 			
				r_child_params.interval.start = pivot+1; 			
				r_child_params.interval.end = hi; 			
				r_child_params.priority = task_param->priority; 			
				os_sem_init(&r_child_params.childlock, 0); 			
				os_tsk_create_ex( quick_sort_task_sem, r_child_params.priority, &r_child_params ); 			
				os_sem_wait(&r_child_params.childlock, 0xffff);
			}
		}
		else{
			//use insertion sort
			insertion_sort(task_param->interval.array.array, lo, hi);
		}
		
	//printf("]");
	}
	
	os_sem_send(&(task_param->childlock));

	//increase_counter(false);
	//printf(">");
	os_tsk_delete_self();
}

void quicksort_sem( array_t array ) {
	array_interval_t interval;
	qsort_task_parameters_t task_param;

	counter = 0;	
	useInsert = array.length / 80;
	if(useInsert < 5) useInsert = 5;
	
	interval.array =  array;
	interval.start     =  0;
	interval.end     =  array.length - 1;

	task_param.interval = interval;
	
	os_tsk_create_ex( quick_sort_task_sem, task_param.priority, &task_param );
	os_sem_wait(task_param.childlock, 0xffff);
}







