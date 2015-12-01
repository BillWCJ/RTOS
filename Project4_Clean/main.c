#include <LPC17xx.h>
#include <RTL.h>
#include <rtl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "GLCD.h"

#include "main.h"
#include "LED.h"
#include "KBD.h"
#include "INT0.h"
#include "Potentiometer.h"

#define BG     White
#define FG     Blue
#define PU	   Purple

#define MAX_BALLS	8
#define MAXX 320
#define MAXY 240
#define MAX_ADC 500 // 1200

volatile bool save_draw = false;
//volatile int balls = 0;
volatile bool create = true;
volatile bool end = false;



OS_SEM freeball;
OS_SEM Create;
OS_SEM Draw;
OS_SEM Rand;
OS_SEM Collide;
OS_SEM Dist;

const unsigned int colourList[15] = {Black, Navy, DarkGreen, DarkCyan, Maroon, Purple, Olive, LightGrey, DarkGrey, Blue, Green, Cyan, Red, Magenta, Yellow};

unsigned short largeBallclear[] = { BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG };

volatile typedef struct {
	volatile double x, y, speed_x, speed_y, old_x, old_y;
	volatile unsigned short color;
	volatile int radius;
	volatile bool active;
	volatile unsigned short bitmap [400]; //[(2*10)*(2*10)];
	volatile unsigned char* clearbitmap; //[(2*10)*(2*10)];
	
	OS_SEM block;
	volatile bool running;
} bounceBall;

volatile bounceBall* ballParam[MAX_BALLS];
volatile int userBall = 0;

// ####################  FUNCTIONS  ####################  

//assign random starting position and speed
void position( bounceBall* ball ){
	os_sem_wait(&Rand, 0xFFFF);
	ball->x = rand()%(MAXX-2*ball->radius);
	ball->y = rand()%(MAXY-2*ball->radius);
	ball->speed_x = 2;//((rand()%3) -1)/2.; // can only be from 0-6 then shifted to -3 to 3
	ball->speed_y = 2;//((rand()%3) -1)/2.;
	os_sem_send(&Rand);			
}

//check the position of the ball. if off the map re store its old value and change direction of the speed
void check_edge( bounceBall* ball){//, int old_x, int old_y ){
	if((ball->y < 0) || (ball->y > (MAXY - 2*ball->radius))){
		ball->y = ball->old_y;
		ball->speed_y = -(ball->speed_y);
	}
	if((ball->x < 0) || (ball->x > (MAXX - 2*ball->radius))){
		ball->x = ball->old_x;
		ball->speed_x = -(ball->speed_x);
	}
}

//set random size, colour, inital position, speed and ...
void setRandom( bounceBall* ball ){
	int i, j;
	double dx, dy;
	int x0 = ball->x;
	int y0 = ball->y;
	int r, w, h;

	ball->active = true;
	ball->radius = (rand()%7 + 4);
	position(ball);
	ball->color = colourList[rand()%15];
	r = ball->radius;
	w = 2*ball->radius;
	h = 2*ball->radius;
	for (i = (h-1)*w; i > -1; i -= w) {
		for (j = 0; j < w; j++) {
			dx = i/w - r;
			dy = j - r;
					if(dx*dx+dy*dy <= r*r)
						ball->bitmap[i+j] = ball->color;
					else
						ball->bitmap[i+j] = BG;
		}
	}
}

void checkCollision2( void ) {
	int i, j, dx, dy, r;
	//while(1) {
		//collision detection loop thought all the balls and compare it with each other
		os_sem_wait(&Collide, 0xFFFF);
		for (i = 0; i < MAX_BALLS; i++){
			for(j = 0; j < MAX_BALLS; j++){
				if(i != j && ballParam[i]->active && ballParam[j]->active){
					//calculate difference in distance x, y and radial distance between the two task
					dx = (ballParam[i]->x - ballParam[j]->x);
					dy = (ballParam[i]->y - ballParam[j]->y);
					r = (2*ballParam[i]->radius + 2*ballParam[j]->radius)/2;
					
					//destory ball
					if(dx*dx +dy*dy <= r*r){
						ballParam[i]->active = false;
						ballParam[j]->active = false;
						//balls -=2;
						
// 						os_sem_send(&freeball);
// 						os_sem_send(&freeball);
						
						if(ballParam[userBall]->active == false){
							//change user ball to the first active ball
							userBall = 0;
							while(!ballParam[userBall]->active && userBall < MAX_BALLS - 1)
								userBall++; //iterate until an active ball is found
						}
// 						os_sem_wait(&Draw, 0xFFFF);
// 						GLCD_Bitmap ((int)ballParam[i]->x, (int)ballParam[i]->y, 2*ballParam[i]->radius, 2*ballParam[i]->radius, (unsigned char*)ballParam[i]->clearbitmap);
// 						GLCD_Bitmap ((int)ballParam[i]->old_x, (int)ballParam[i]->old_x, 2*ballParam[i]->radius, 2*ballParam[i]->radius, (unsigned char*)ballParam[i]->clearbitmap);
// 					
// 						GLCD_Bitmap ((int)ballParam[j]->x, (int)ballParam[j]->y, 2*ballParam[j]->radius, 2*ballParam[j]->radius, (unsigned char*)ballParam[j]->clearbitmap);
// 						GLCD_Bitmap ((int)ballParam[j]->old_x, (int)ballParam[j]->old_x, 2*ballParam[j]->radius, 2*ballParam[j]->radius, (unsigned char*)ballParam[j]->clearbitmap);
// 						os_sem_send(&Draw);
					}
				}
			}
		}
		os_sem_send(&Collide);
	//}
}
 
// ################### TASKS ###################
__task void ball_task( void* ballp ){
	//int old_x = 0, old_y = 0;
	bounceBall* ball = (bounceBall*)ballp;
	ball->running = true;
	os_sem_wait(&ball->block, 0xFFFF);
//   os_sem_wait(&Draw, 0xFFFF);
//  	GLCD_Bitmap ((int)ball->x, (int)ball->y, 2*ball->radius, 2*ball->radius, (unsigned char*)ball->clearbitmap);
//  	GLCD_Bitmap ((int)ball->old_x, (int)ball->old_x, 2*ball->radius, 2*ball->radius, (unsigned char*)ball->clearbitmap);
//  	os_sem_send(&Draw);	
	setRandom(ball);
	while(ball->active){
		os_sem_wait(&Draw, 0xFFFF);
 		GLCD_Bitmap ((int)ball->x, (int)ball->y, 2*ball->radius, 2*ball->radius, (unsigned char*)ball->clearbitmap);
		
		//move the ball
		ball->x += ((double)ADCValue()/MAX_ADC)*ball->speed_x;
		ball->y += ((double)ADCValue()/MAX_ADC)*ball->speed_y;
		check_edge(ball);//, old_x, old_y);
		
		//assign the new value to the old and continue
		ball->old_x = ball->x;
		ball->old_y = ball->y;
		
 		GLCD_Bitmap ((int)ball->x, (int)ball->y, 2*ball->radius, 2*ball->radius, (unsigned char*)ball->bitmap);
 		os_sem_send(&Draw);
		
		checkCollision2();
	}
	
	//clear the ball when it dies
  os_sem_wait(&Draw, 0xFFFF);
 	GLCD_Bitmap ((int)ball->x, (int)ball->y, 2*ball->radius, 2*ball->radius, (unsigned char*)ball->clearbitmap);
 	GLCD_Bitmap ((int)ball->old_x, (int)ball->old_x, 2*ball->radius, 2*ball->radius, (unsigned char*)ball->clearbitmap);
 	os_sem_send(&Draw);

	os_sem_send(&freeball);
	ball->running = false;
	os_sem_send(&ball->block);
	os_tsk_delete_self();
}

// This dummy thread constantly asks to read the poti. when the data becomes ready
// ADC_Done will be 1;
__task void readPoti_task( void ){
	while( 1 ){
		ADCConvert();
	}
}

__task void ballCountLED( void ){
	int i;
	while(1){
		for(i = 0; i < MAX_BALLS; i++){
			if (ballParam[i]->active)
				turnOnLED(i);
			else
				turnOffLED(i);
		}
	}
}

__task void joyControl( void ) {
	uint32_t joy = KBD_UP;
	while(1){
		joy = get_button();
		//nudge the ball. basically override the existing speed
		if(joy == KBD_UP)
				ballParam[userBall]->speed_y = 1;
		else if(joy == KBD_RIGHT)
				ballParam[userBall]->speed_x = 1;
		else if(joy == KBD_DOWN)
				ballParam[userBall]->speed_y = -1;
		else if(joy == KBD_LEFT)
				ballParam[userBall]->speed_x = -1;
	}
}

// __task void checkCollision( void ) {
// 	int i, j, dx, dy, r;
// 	while(1) {
// 		//collision detection loop thought all the balls and compare it with each other
// 		os_sem_wait(&Collide, 0xFFFF);
// 		for (i = 0; i < MAX_BALLS; i++){
// 			for(j = 0; j < MAX_BALLS; j++){
// 				if(i != j && ballParam[i]->active && ballParam[j]->active){
// 					//calculate difference in distance x, y and radial distance between the two task
// 					dx = (ballParam[i]->x - ballParam[j]->x);
// 					dy = (ballParam[i]->y - ballParam[j]->y);
// 					r = (2*ballParam[i]->radius + 2*ballParam[j]->radius)/2;
// 					
// 					//destory ball
// 					if(dx*dx +dy*dy <= r*r){
// 						ballParam[i]->active = false;
// 						ballParam[j]->active = false;
// 						//balls -=2;
// 						
// 						os_sem_send(&freeball);
// 						os_sem_send(&freeball);
// 						
// 						if(ballParam[userBall]->active == false){
// 							//change user ball to the first active ball
// 							userBall = 0;
// 							while(!ballParam[userBall]->active && userBall < MAX_BALLS - 1)
// 								userBall++; //iterate until an active ball is found
// 						}
// // 						os_sem_wait(&Draw, 0xFFFF);
// // 						GLCD_Bitmap ((int)ballParam[i]->x, (int)ballParam[i]->y, 2*ballParam[i]->radius, 2*ballParam[i]->radius, (unsigned char*)ballParam[i]->clearbitmap);
// // 						GLCD_Bitmap ((int)ballParam[i]->old_x, (int)ballParam[i]->old_x, 2*ballParam[i]->radius, 2*ballParam[i]->radius, (unsigned char*)ballParam[i]->clearbitmap);
// // 					
// // 						GLCD_Bitmap ((int)ballParam[j]->x, (int)ballParam[j]->y, 2*ballParam[j]->radius, 2*ballParam[j]->radius, (unsigned char*)ballParam[j]->clearbitmap);
// // 						GLCD_Bitmap ((int)ballParam[j]->old_x, (int)ballParam[j]->old_x, 2*ballParam[j]->radius, 2*ballParam[j]->radius, (unsigned char*)ballParam[j]->clearbitmap);
// // 						os_sem_send(&Draw);
// 					}
// 				}
// 			}
// 		}
// 		os_sem_send(&Collide);
// 	}
// }

__task void init_task( void ) {
  int debouncecount = 0, firstfree = 0, i;
	bool debounced = true;
	// Increase the priority to intilize the task first, then allow them to start
	os_tsk_prio_self ( 2 );
		
	//initialize semaphores
	os_sem_init(&Draw, 1);
	os_sem_init(&Rand, 1);
	os_sem_init(&Create, 1);
	os_sem_init(&Collide, 1);
	os_sem_init(&Dist, 1);
	os_sem_init(&freeball, 8);
	
	//first ball is created below because "create" is initally set to true
	
	// Launch a multi thread checking system
	os_tsk_create (readPoti_task, 1);
	os_tsk_create (ballCountLED, 1);
	os_tsk_create (joyControl, 1);
	//os_tsk_create (checkCollision, 1);
	
	os_tsk_prio_self ( 1 );
	
	for(i = 0; i < 8; i++){
		
		os_sem_init(&(ballParam[i]->block), 1);
		
		//os_tsk_create_ex (ball_task, 1, (void*)ballParam[firstfree]);
	}
	
	while(1){
// 		create = true;
// 		debounced = true;
		if(!create){
			debouncecount++;
			if(debouncecount > 100){
				debounced = true;
				debouncecount = 0;
			}
		}
			
		//create ball
		if(create){
			if(debounced){
				debounced = false;
				if (true){//balls < MAX_BALLS){
					
					os_sem_wait(&freeball, 0xFFFF);
					firstfree = 0;
					while (ballParam[firstfree] != NULL && ballParam[firstfree]->active && firstfree < MAX_BALLS - 1)
						firstfree++;//loop through to find the first free ball
					
					if(!ballParam[firstfree]->active){
						//bounceBall* ball = (ballParam[firstfree]);
						os_sem_wait(&Create, 0xFFFF);
						
// 						ballParam[firstfree] = (bounceBall*)malloc(sizeof(bounceBall));
// 						ballParam[firstfree]->active = true;
// 						ballParam[firstfree]->clearbitmap = (unsigned char*)largeBallclear;
						
						userBall = firstfree;
						os_tsk_create_ex (ball_task, 1, (void*)ballParam[firstfree]);
						//balls++;
						
						os_sem_send(&Create);
					}
				}
			}
			create = false;
		}
	}
}

void INT0Init() {

	// P2.10 is related to the INT0 or the push button.
	// P2.10 is selected for the GPIO 
	LPC_PINCON->PINSEL4 &= ~(3<<20); 

	// P2.10 is an input port
	LPC_GPIO2->FIODIR   &= ~(1<<10); 

	// P2.10 reads the falling edges to generate the IRQ
	// - falling edge of P2.10
	LPC_GPIOINT->IO2IntEnF |= (1 << 10);

	// IRQ is enabled in NVIC. The name is reserved and defined in `startup_LPC17xx.s'.
	// The name is used to implemet the interrupt handler above,
	NVIC_EnableIRQ( EINT3_IRQn );
}

// INT0 interrupt handler
void EINT3_IRQHandler( void ) {
		create = true;
	// Check whether the interrupt is called on the falling edge. GPIO Interrupt Status for Falling edge.
		LPC_GPIOINT->IO2IntClr |= (1 << 10); // clear interrupt condition
}

int main( void ) {
	int i;
  // system initalization
	SystemInit();
	SystemCoreClockUpdate();
	
	// GLCD initalization
	GLCD_Init();
	GLCD_Clear(BG);
	
	// peripherals initialization
	LEDInit();
	INT0Init();
	ADCInit();
	KBD_Init();
	
	//allocation all the ball structs and deactivate it
	for( i = 0; i < MAX_BALLS; i++){
		ballParam[i] = (bounceBall*)malloc(sizeof(bounceBall));
		ballParam[i]->active = false;
 		//ballParam[i]->bitmap = (unsigned char*) malloc(sizeof(unsigned short));
 		ballParam[i]->clearbitmap = (unsigned char*)largeBallclear;
	}

	os_sys_init(init_task);
}
