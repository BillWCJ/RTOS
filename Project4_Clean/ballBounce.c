#include <LPC17xx.h>
#include <RTL.h>
#include <rtl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ballBounce.h"

#define USE_INSERTION_SORT	150
#define MAX_TASK	40

size_t useInsert=0;
OS_SEM mainLock;
OS_MUT mut;
volatile int counter;

// Local prototypes
void generate_quickSort_sem(array_t array, size_t start, size_t end, unsigned char priority);

//structs
typedef struct {
	array_t array;
	size_t start;
	size_t end;
} array_interval_t;

typedef struct{
	array_interval_t interval;
	unsigned char priority;
} qsort_task_parameters_t;

void increase_counter(void){
	os_mut_wait (&mut, 0xffff);
	counter++;
	os_mut_release(&mut);
}

void decrease_counter(void){
	os_mut_wait (&mut, 0xffff);
	counter--;
	//unlock main and let it exit
	if(counter == 0) 
		os_sem_send(&mainLock);
	os_mut_release (&mut);
}

void swap(array_type* i, array_type* j) {
    array_type temp = *j;
    *j = *i;
    *i = temp;
}

size_t median_of_three(array_type arr[], size_t lo, size_t hi){
    size_t middle_index = lo + (hi -lo)/2;
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
		array_type* j;
		array_type* i = &(arr[start+1]);
		array_type* final = &(arr[end]);
		array_type* begin = &(arr[start]);
	  array_type temp;
	
    for (; i <= final; i++){
        j = i;
        while(j > begin && *(j-1) > *(j)){
						temp = *j;
						*j = *(j-1);
						*(j-1) = temp;
            j--;
        }
    }
}

size_t partition(array_type* arr, size_t i, size_t j){
		array_type temp;
    array_type pivot = arr[rand()%(j-i)+i];//arr[median_of_three(arr, i ,j)];
		array_type* a = &(arr[i-1]);
		array_type* b = &(arr[j+1]);
	
    while (true) {
        while (*(++a) < pivot);
        while (*(--b) > pivot);
        if (a < b){
						temp = *a;
						*a = *b;
						*b = temp;
				}
        else
            return (b - &(arr[i]))/sizeof(array_type) + i;
    };
}

__task void quick_sort_task_sem( void* void_ptr) {
    qsort_task_parameters_t *task_param = (qsort_task_parameters_t*)void_ptr;
    size_t lo = task_param->interval.start;
    size_t hi = task_param->interval.end;
    size_t pivot;
		qsort_task_parameters_t* l_child_params, *r_child_params;

		// is the array already sorted
		while (lo < hi){
				if ((hi - lo <= USE_INSERTION_SORT) || (counter >= MAX_TASK)) {
						//use insertion sort
						insertion_sort(task_param->interval.array.array, lo, hi);
						break;
				}
				else{
					//less than use insertion sort limit. use quick sort still
						pivot = partition(task_param->interval.array.array, lo, hi);
// 						os_mut_wait (&mut, 0xffff);
// 						l_child_params = (qsort_task_parameters_t*)malloc(sizeof(qsort_task_parameters_t));
// 						os_mut_release(&mut);
// 						l_child_params->interval.array = task_param->interval.array; 			
// 						l_child_params->interval.start = lo; 			
// 						l_child_params->interval.end = pivot; 			
// 						l_child_params->priority = task_param->priority; 	
// 						increase_counter();
// 						if (os_tsk_create_ex( quick_sort_task_sem, l_child_params->priority, l_child_params ) == 0){	
// 							os_mut_wait (&mut, 0xffff);
// 							free(l_child_params);
// 							os_mut_release(&mut);
// 							decrease_counter();
// 							insertion_sort(task_param->interval.array.array, lo, pivot);
// 						}
					
						os_mut_wait (&mut, 0xffff);
						r_child_params = (qsort_task_parameters_t*)malloc(sizeof(qsort_task_parameters_t));
						os_mut_release(&mut);
						r_child_params->interval.array = task_param->interval.array; 			
						r_child_params->interval.start = pivot+1; 			
						r_child_params->interval.end = hi; 			
						r_child_params->priority = task_param->priority;
				
						increase_counter();
						if (os_tsk_create_ex( quick_sort_task_sem, r_child_params->priority, r_child_params ) == 0){
							os_mut_wait (&mut, 0xffff);
							free(r_child_params);
							os_mut_release(&mut);
							decrease_counter();					
							insertion_sort(task_param->interval.array.array, pivot +1, hi);
						}
						hi = pivot;
				}
		}
		
		os_mut_wait (&mut, 0xffff);
    free(task_param);
		os_mut_release(&mut);
    decrease_counter();
    os_tsk_delete_self();
}

void quicksort_sem( array_t array) {
		qsort_task_parameters_t* task_param;
		os_mut_wait (&mut, 0xffff);
		task_param =(qsort_task_parameters_t*) malloc(sizeof(qsort_task_parameters_t));
		os_mut_release(&mut);
		task_param->interval.array = array;
		task_param->interval.start = 0;
		task_param->interval.end = array.length-1;
		task_param->priority = 1;

    os_sem_init(&mainLock,0);
		os_mut_init (&mut);

		counter = 1;		
		os_tsk_create_ex(quick_sort_task_sem, task_param->priority, task_param);
		os_sem_wait(&mainLock, 0xffff);
}


__task void quick_sort_task( void* void_ptr) {
	qsort_task_parameters_t *task_param = (qsort_task_parameters_t*)void_ptr;
	size_t lo = task_param->interval.start;
	size_t hi = task_param->interval.end;
	size_t pivot;

	if (lo < hi){
		if(lo+useInsert < hi){
			//less than use insertion sort limit. use quick sort still
			pivot = partition(task_param->interval.array.array, lo, hi);
			
			if(lo < pivot){
				qsort_task_parameters_t l_child_params;

				l_child_params.interval.array = task_param->interval.array; 			
				l_child_params.interval.start = lo; 			
				l_child_params.interval.end = pivot; 			
				l_child_params.priority = task_param->priority + 1; 	
				os_tsk_create_ex( quick_sort_task, l_child_params.priority, &l_child_params ); 			
			}
			if(pivot +1 < hi){
				qsort_task_parameters_t r_child_params;
				r_child_params.interval.array = task_param->interval.array; 			
				r_child_params.interval.start = pivot+1; 			
				r_child_params.interval.end = hi; 			
				r_child_params.priority = task_param->priority + 1;
				os_tsk_create_ex( quick_sort_task, r_child_params.priority, &r_child_params ); 			
			}
		}
		else{
			//use insertion sort
			insertion_sort(task_param->interval.array.array, lo, hi);
		}
	}
	os_tsk_delete_self();
}

void quicksort( array_t array ) {
	qsort_task_parameters_t task_param;
	array_interval_t interval;
	
	useInsert = array.length / 80;
	if(useInsert < 5) useInsert = 5;
	
	counter = 0;
	interval.array = array;
	interval.start = 0;
	interval.end = array.length - 1;
	task_param.interval = interval;
	task_param.priority = 10;
	os_tsk_create_ex(quick_sort_task, task_param.priority, &task_param );
}

