/* ----------------------------------------------------------------------
* Copyright (C) 2010-2012 ARM Limited. All rights reserved.
*
* $Date:         17. January 2013
* $Revision:     V1.4.0
*
* Project:       CMSIS DSP Library
* Title:         arm_class_marks_example_f32.c
*
* Description:   Example code to calculate Minimum, Maximum
*                Mean, std and variance of marks obtained in a class
*
* Target Processor: Cortex-M4/Cortex-M3
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*   - Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   - Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.
*   - Neither the name of ARM LIMITED nor the names of its contributors
*     may be used to endorse or promote products derived from this
*     software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
* -------------------------------------------------------------------- */

/**
 * @ingroup groupExamples
 */

/**
 * @defgroup ClassMarks Class Marks Example
 *
 * \par Description:
 * \par
 * Demonstrates the use the Maximum, Minimum, Mean, Standard Deviation, Variance
 * and Matrix functions to calculate statistical values of marks obtained in a class.
 *
 * \note This example also demonstrates the usage of static initialization.
 *
 * \par Variables Description:
 * \par
 * \li \c testMarks_f32 points to the marks scored by 20 students in 4 subjects
 * \li \c max_marks     Maximum of all marks
 * \li \c min_marks     Minimum of all marks
 * \li \c mean          Mean of all marks
 * \li \c var           Variance of the marks
 * \li \c std           Standard deviation of the marks
 * \li \c numStudents   Total number of students in the class
 *
 * \par CMSIS DSP Software Library Functions Used:
 * \par
 * - arm_mat_init_f32()
 * - arm_mat_mult_f32()
 * - arm_max_f32()
 * - arm_min_f32()
 * - arm_mean_f32()
 * - arm_std_f32()
 * - arm_var_f32()
 *
 * <b> Refer  </b>
 * \link arm_class_marks_example_f32.c \endlink
 *
 */


/** \example arm_class_marks_example_f32.c
  */
//#include "arm_math.h"
//#include "w52jiang_pshering_1.c"

//int32_t main()
//{
//    Test();
//}


#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
//#include "../GLCD/Font_6x8_h.h"
//#include "../GLCD/Font_16x24_h.h"
//#include "../GLCD/GLCD.h"
//#include "../GLCD/GLCD_Scroll.h"
//#include "../GLCD/GLCD_Scroll.c"
//#include "../GLCD/GLCD_SPI_LPC1700.c"
//#include "../GLCD/Retarget.c"
#include "bst.h"
//#include "bst.c"

S32 value_array[100] = {
    534, 6415,  465, 4459, 6869, 4442, 5840, 4180, 7450, 9265,
    23, 2946, 3657, 3003,   29, 8922, 2199, 6973, 2344, 1802,
    9248, 5388, 2198, 2838, 1117, 5346, 4619, 8334, 9593, 2288,
    7346, 9252, 8169, 4138, 7479,  366, 5064, 6872,   -3, 8716,
    8089,   15, 5337, 8700, 8128, 6673, 5395, 7772, 5792, 3379,
    2438, 2184, 1176, 6083, 6572,  915, 1635, 6457, 3729, 7791,
    7642, 1548, 6267, 6562, 6477, 6026, 7460, 7226, 1994, 6519,
    7660, 3018, 2205,  559, 1347, 1647, 8778, 3864, 2543, 8370,
    1152,  865,  860, 8735, 4782,  574, 5858, 7089, 2096, 7449,
    1310, 3043, 8247, 6382, 2470, 3072, 1297, 7396, 7073,  140};

S32 erase_array[5][20] = {
    { 915, 1802, 1994, 6083,  865, 8735, 6457, 8334, 4459, 3003, 2198, 2470, 7642,   15, 7772, 1152,   29, 2096,  574, 6415},
    {7396, 5858, 4442, 6872, 8128, 2838,  465, 6477, 8247, 6572, 2946, 1297, 3729, 4138, 5064, 8778, 4619, 5346,   -3, 3657},
    {1347, 2288, 7791, 7073, 5792, 3018,  366, 7449, 2543, 8089, 4180, 6026, 3864, 5395, 7226, 1117,   23, 7089, 1635, 6267},
    {8700, 3072, 7660, 6673, 2438, 3043, 1548, 4782, 6519, 7460,  559,  860, 6562, 9593, 1647, 1310, 3379, 8716, 8922, 7450},
    {9265, 6973, 8169, 5388,  140, 6869, 2344, 9252, 2184, 9248,  534, 2199, 6382, 7479, 8370, 7346, 5337, 5840, 2205, 1176}};


int32_t main(){
		int erase_index = 0;
    int i = 0, min = 0, max = 0;
    bst_t* tree;
    bst_init(tree);
	
    printf("Expected Output:\n");
    printf("   Before first group is erased                -3,        9593 \n");
    printf("   After first group is erased                 -3,        9593\n");
    printf("   After second group is erased                23,        9593\n");
    printf("   After third group is erased                140,        9593\n");
    printf("   After fourth group is erased               140,        9265\n");
    printf("   When the tree is empty              2147483647, -2147483648\n\n\n\n");

    for (; i < 100; i++){
        if(bst_insert(tree, value_array[i]) == __FALSE)
            printf("Error Inserting: %d\n", value_array[i]);
    }
		min = bst_min(tree);
		max = bst_max(tree);
    printf("   Before first group is erased                %d,        %d \n", bst_min(tree), bst_max(tree));

    erase_index = 0;
    i=0;
    for(; i < 20; i++){
        if(bst_erase(tree, erase_array[erase_index][i]) == __TRUE)
            ;//printf("Error deleting: %d\n", erase_array[erase_index][i]);
    }
		min = bst_min(tree);
		max = bst_max(tree);
    printf("   After first group is erased                %d,        %d \n", bst_min(tree), bst_max(tree));

    erase_index = 1;
    i=0;
    for(; i < 20; i++){
        if(bst_erase(tree, erase_array[erase_index][i]) == __TRUE)
            ;//printf("Error deleting: %d\n", erase_array[erase_index][i]);
    }
		min = bst_min(tree);
		max = bst_max(tree);
    printf("   After second group is erased                %d,        %d \n", bst_min(tree), bst_max(tree));

    erase_index = 2;
    i=0;
    for(; i < 20; i++){
        if(bst_erase(tree, erase_array[erase_index][i]) == __TRUE)
            ;//printf("Error deleting: %d\n", erase_array[erase_index][i]);
    }
		min = bst_min(tree);
		max = bst_max(tree);
    printf("   After third group is erased                %d,        %d \n", bst_min(tree), bst_max(tree));

    erase_index = 3;
    i=0;
    for(; i < 20; i++){
        if(bst_erase(tree, erase_array[erase_index][i]) == __TRUE)
            ;//printf("Error deleting: %d\n", erase_array[erase_index][i]);
    }
		min = bst_min(tree);
		max = bst_max(tree);
    printf("   After fourth group is erased                %d,        %d \n", bst_min(tree), bst_max(tree));

    erase_index = 4;
    i=0;
    for(; i < 20; i++){
        if(bst_erase(tree, erase_array[erase_index][i]) == __TRUE)
            ;//printf("Error deleting: %d\n", erase_array[erase_index][i]);
    }
		min = bst_min(tree);
		max = bst_max(tree);
    printf("   When the tree is empty                %d,        %d \n", bst_min(tree), bst_max(tree));
		return 1;
}
