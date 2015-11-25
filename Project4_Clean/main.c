#include <LPC17xx.h>
#include <RTL.h>
#include <rtl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "GLCD_Scroll.h"
#include "GLCD.h"

#include "LED.h"
#include "KBD.h"
#include "INT0.h"
#include "Potentiometer.h"

#define BG     White
#define FG     Blue
#define PU	   Purple

#define MAX_BALLS 8
#define MAXX 320
#define MAXY 240
#define MAX_ADC 1200


volatile bool save_draw = true;
volatile int balls = 0;
volatile bool create = false;
volatile bool end = false;


volatile int adc = 1200;

OS_MUT Ball;
OS_SEM Create;
OS_SEM Draw;
OS_SEM Rand;

unsigned short clear_bitmap[] = { BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																	BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																	BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																	BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																	BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																	BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																	BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																	BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																	BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																	BG, BG, BG, BG, BG, BG, BG, BG, BG, BG};

unsigned short cross_bitmap[] = { BG, BG, BG, PU, PU, PU, PU, BG, BG, BG,
																	BG, BG, PU, PU, PU, PU, PU, PU, BG, BG,
																	BG, PU, PU, PU, PU, PU, PU, PU, PU, BG,
																	PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
																	PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
																	PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
																	PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
																	BG, PU, PU, PU, PU, PU, PU, PU, PU, BG,
																	BG, BG, PU, PU, PU, PU, PU, PU, BG, BG,
																	BG, BG, BG, PU, PU, PU, PU, BG, BG, BG};		
const int size_x = 10, size_y = 10;

// Stop blinking
unsigned char stopBlink = 0;

// All references to sections come from LPC 1768 User Manual
//     http:// www.nxp.com/documents/user_manual/UM10360.pdf

// Functions
void button_pressed_callback(void){
	//change what you want the function to do when the button is pressed
	//stopBlink = stopBlink ^ 1 ;
	create = true;
}

void check_edge(int x, int y, int old_x, int old_y, int size_x, int size_y, int* speed_x, int* speed_y){
	if(y < 0 || y > (MAXY - size_y)){
		y = old_y;
		*speed_y = -(*speed_y);
	}
	if(x < 0 || x > (MAXX - size_x)){
		x = old_x;
		*speed_x = -(*speed_x);
	}
}

void draw_ball(int x, int y, int old_x, int old_y, int size_x, int size_y, unsigned char*clear_bitmap, unsigned char*cross_bitmap){
	os_sem_wait(&Draw, 0xFFFF);
	GLCD_Bitmap (old_x, old_y, size_x, size_y, (unsigned char*)clear_bitmap);
	GLCD_Bitmap (x, y, size_x, size_y, (unsigned char*)cross_bitmap);
	os_sem_send(&Draw);
}

void DrawCircle(int x0, int y0, int radius)
{
  int x = radius;
  int y = 0;
  int decisionOver2 = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0

  while( y <= x )
  {
    GLCD_PutPixel( x + x0,  y + y0); // Octant 1
    GLCD_PutPixel( y + x0,  x + y0); // Octant 2
    GLCD_PutPixel(-x + x0,  y + y0); // Octant 4
    GLCD_PutPixel(-y + x0,  x + y0); // Octant 3
    GLCD_PutPixel(-x + x0, -y + y0); // Octant 5
    GLCD_PutPixel(-y + x0, -x + y0); // Octant 6
    GLCD_PutPixel( x + x0, -y + y0); // Octant 8
    GLCD_PutPixel( y + x0, -x + y0); // Octant 7
    y++;
    if (decisionOver2<=0)
    {
      decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
    }
    else
    {
      x--;
      decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
    }
  }
}

void draw_ball2(int x, int y, int old_x, int old_y, int r, unsigned short colour){
// 	int i, j;
// 	double dx, dy;
	os_sem_wait(&Draw, 0xFFFF);
	// clear old circle
	GLCD_SetTextColor(BG);
	DrawCircle(old_x, old_y, r);
// 	for (i = old_x-r; i<old_x+r; i++){
// 		for (j = old_y-r; j<old_y+r; j++){
// 			dx = i-old_x;
// 			dy = j-old_y;
// 			if((dx*dx + dy*dy) <= r*r)
// 				GLCD_PutPixel(i, j);
// 		}
// 	}
	
	//draw circle
	GLCD_SetTextColor(colour);
// 	for (i = x - r; i < x+r; i++){
// 		for (j = y - r; j < y+r; j++){
// 			dx = i-x;
// 			dy = j-y;
// 			if((dx*dx + dy*dy) <= r*r)
// 				GLCD_PutPixel(i, j);
// 		}
// 	}

	DrawCircle(x, y, r);
	os_sem_send(&Draw);
}

// Tasks

__task void ball_task(void* arg){
	int x = 0, y = 0, old_x = 0, old_y = 0, speed_x = 0, speed_y = 0;
						
	os_sem_wait(&Rand, 0xFFFF);
	x = rand()%(MAXX-size_x);
	y = rand()%(MAXY-size_y);
	speed_x = (rand()%7) -3; // can only be from 0-6 then shifted to -3 to 3
	speed_y = (rand()%7) -3;
	os_sem_send(&Rand);																		
							
	while(1){
		x += ((double)adc/MAX_ADC)*speed_x;
		y += ((double)adc/MAX_ADC)*speed_y;
		check_edge(x, y, old_x, old_y, size_x, size_y, &speed_x, &speed_y);
		
		if(save_draw && (x != old_x || y != old_y)){
			draw_ball(x, y, old_x, old_y, size_x, size_y, (unsigned char*)clear_bitmap, (unsigned char*)cross_bitmap);
			
			//draw_ball2(x+(size_x/2), y+(size_y/2), old_x+(size_x/2), old_y+(size_y/2), ((size_x+size_y)/4), FG);
			
			old_x = x;
			old_y = y;	
		}
	}
	os_tsk_delete_self();
}

__task void user_task(void* arg){
	int x = 0, y = 0, old_x = 0, old_y = 0, speed_x = 0, speed_y = 0;
	uint32_t val = KBD_UP;
																														
	os_sem_wait(&Rand, 0xFFFF);
	x = rand()%(MAXX-size_x);
	y = rand()%(MAXY-size_y);
	speed_x = (rand()%7) -3;
	speed_y = (rand()%7) -3;
	os_sem_send(&Rand);																		
																			
	while(1){
		val = get_button();
		
		x += ((double)adc/MAX_ADC)*speed_x;
		y += ((double)adc/MAX_ADC)*speed_y;
		check_edge(x, y, old_x, old_y, size_x, size_y, &speed_x, &speed_y);
		
		if(val == KBD_UP && y < (MAXY - size_y))
				speed_y += 1;
		else if(val == KBD_RIGHT && x < (MAXX - size_x))
				speed_x += 1;
		else if(val == KBD_DOWN && y > 0)
				speed_y += -1;
		else if(val == KBD_LEFT && x > 0)
				speed_x += -1;
		
		if(speed_x > 3)
			speed_x = 3;
		else if(speed_x < -3)
			speed_x = -3;
		
		if(speed_y > 3)
			speed_y = 3;
		else if(speed_y < -3)
			speed_y = -3;
			
		if(save_draw && (x != old_x || y != old_y)){
			draw_ball(x, y, old_x, old_y, size_x, size_y, (unsigned char*)clear_bitmap, (unsigned char*)cross_bitmap);
			
			//draw_ball2(x+(size_x/2), y+(size_y/2), old_x+(size_x/2), old_y+(size_y/2), ((size_x+size_y)/4), FG);
			
			//draw_ball2(x+(size_x/2), y+(size_y/2), old_x+(size_x/2), old_y+(size_y/2), 10, FG);
			
			old_x = x;
			old_y = y;
		}
	}
	os_tsk_delete_self();
}

// This dummy thread constantly asks to read the poti. when the data becomes ready
// ADC_Done will be 1;
__task void readPoti_task(void){
	
	while( 1 ){
		ADCConvert();
		//Now wait for the other threads.
		os_dly_wait( 100 );
	}
}

__task void init_task( void ) {
	// Increase the priority to intilize the task first, then allow them to start
	os_tsk_prio_self ( 2 );
	os_sem_init(&Draw, 1);
	os_sem_init(&Rand, 1);
	os_sem_init(&Create, 1);
	
	// Launche a thread to constantly read the poti
	os_tsk_create ( readPoti_task, 1);
	
	balls = 1;
	turnOnLED(balls-1);
	os_tsk_create_ex(user_task, 1, NULL);
	os_tsk_prio_self ( 1 );
	
	while(1){
		unsigned short int pot = 0; 
		if(create){
			if(balls < 8){
				os_sem_wait(&Draw, 0xFFFF);
				os_tsk_create_ex (ball_task, 1, NULL);
				create = false;
				os_sem_send(&Draw);
				balls++;
				turnOnLED(balls-1);
			}
			else{	
				os_sem_wait(&Draw, 0xFFFF);
				create = false;
				os_sem_send(&Draw);
			}
			os_dly_wait(100);
		}
		
		pot = ADCValue();
		if(pot != adc)
			adc = pot;
	}
}

int main( void ) {
  //initalization
	SystemInit();
	SystemCoreClockUpdate();
	GLCD_Init();
	LEDInit();
	INT0Init(button_pressed_callback);
	ADCInit();
	KBD_Init();
	GLCD_Clear(BG);

	os_sys_init(init_task);
		while(1){
	}
}

// __task void blink_task( void *param ) {
// 	unsigned char currentLED = *((unsigned char *) param);
// 	
// 	// Pause until the next task is cleared. Although the priority in the init_task
// 	// doesn't allow any blink_task to be started before the initializtion compltes,
// 	// the below check makes the code safer.
// 	while( nextTaskArray[currentLED] == 0 ) {
// 		// Busy waiting
// 	}

// 	while ( 1 ) {

// 		// Wait for the flag 0x0001
// 		os_evt_wait_and( 0x0001, 0xffff );

// 		turnOnLED   ( currentLED );

// 		// Stop to blink
// 		while ( stopBlink ) {
// 			// Wait for for the LED to turn off
// 			os_tsk_pass();
// 		}
// 		// Delay x clock ticks. x is set by the poti
// 		os_dly_wait( (ADCValue() >> 3 ) + 1 );

// 		turnOffLED( currentLED );

// 		// Call common signal function
// 		os_evt_set  ( 0x0001, nextTaskArray[ currentLED ] );
// 		
// 		//pass the control to the other thread.
// 		os_tsk_pass();
// 	}
// }

