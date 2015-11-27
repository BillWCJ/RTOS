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
volatile int balls = 0;
volatile bool create = true;
volatile bool end = false;

OS_SEM Create;
OS_SEM Draw;
OS_SEM Rand;
OS_SEM Collide;

const unsigned short colourList[] = {Black, Navy, DarkGreen, DarkCyan, Maroon, Purple, Olive, LightGrey, DarkGrey, Blue, Green, Cyan, Red, Magenta, Yellow};

// unsigned short smallBall[] = { BG, BG, BG, PU, PU, PU, PU, BG, BG, BG,
// 																BG, BG, PU, PU, PU, PU, PU, PU, BG, BG,
// 																BG, PU, PU, PU, PU, PU, PU, PU, PU, BG,
// 																PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
// 																PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
// 																PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
// 																PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
// 																BG, PU, PU, PU, PU, PU, PU, PU, PU, BG,
// 																BG, BG, PU, PU, PU, PU, PU, PU, BG, BG,
// 																BG, BG, BG, PU, PU, PU, PU, BG, BG, BG};

// unsigned short mediumBall[] = { BG, BG, BG, BG, BG, BG, PU, PU, PU, BG, BG, BG, BG, BG, BG,
// 																BG, BG, BG, BG, BG, PU, PU, PU, PU, PU, BG, BG, BG, BG, BG,
// 																BG, BG, BG, BG, PU, PU, PU, PU, PU, PU, PU, BG, BG, BG, BG,
// 																BG, BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG, BG, BG,
// 																BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG, BG,
// 																BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG,
// 																PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
// 																PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
// 																PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
// 																BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG,
// 																BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG, BG,
// 																BG, BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG, BG, BG,
// 																BG, BG, BG, BG, PU, PU, PU, PU, PU, PU, PU, BG, BG, BG, BG,
// 																BG, BG, BG, BG, BG, PU, PU, PU, PU, PU, BG, BG, BG, BG, BG,
// 																BG, BG, BG, BG, BG, BG, PU, PU, PU, BG, BG, BG, BG, BG, BG };

// 								
// unsigned short largeBall[] = { BG, BG, BG, BG, BG, BG, BG, BG, PU, PU, PU, PU, BG, BG, BG, BG, BG, BG, BG, BG,
// 																BG, BG, BG, BG, BG, BG, BG, PU, PU, PU, PU, PU, PU, BG, BG, BG, BG, BG, BG, BG,
// 																BG, BG, BG, BG, BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, BG, BG, BG, BG, BG, BG,
// 																BG, BG, BG, BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG, BG, BG, BG, BG,
// 																BG, BG, BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG, BG, BG, BG,
// 																BG, BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG, BG, BG,
// 																BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG, BG,
// 																BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG,
// 																PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
// 																PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
// 																PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
// 																PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
// 																BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU,
// 																BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG,
// 																BG, BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG, BG,
// 																BG, BG, BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG, BG, BG,
// 																BG, BG, BG, BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG, BG, BG, BG,
// 																BG, BG, BG, BG, BG, BG, PU, PU, PU, PU, PU, PU, PU, PU, PU, BG, BG, BG, BG, BG,
// 																BG, BG, BG, BG, BG, BG, BG, PU, PU, PU, PU, PU, PU, BG, BG, BG, BG, BG, BG, BG,
// 																BG, BG, BG, BG, BG, BG, BG, BG, PU, PU, PU, PU, BG, BG, BG, BG, BG, BG, BG, BG };

// unsigned short smallBallclear[] = { BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																		BG, BG, BG, BG, BG, BG, BG, BG, BG, BG};

// unsigned short mediumBallclear[] = { BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
// 																			BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG };

								
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
//  	volatile int size_x ;
//  	volatile int size_y ;
	volatile double x;
	volatile double y;
	volatile double speed_x;
	volatile double speed_y;

	volatile char color;
	volatile int radius;
	volatile bool active;
	volatile unsigned short bitmap [400]; //[(2*10)*(2*10)];
	volatile unsigned char* clearbitmap; //[(2*10)*(2*10)];
} bounceBall;

volatile bounceBall* ballParam[8];
volatile int userBall = 0;

// Stop blinking
unsigned char stopBlink = 0;

// ####################  FUNCTIONS  ####################  

//assign random starting position and speed
void position( bounceBall* ballParam ){
	os_sem_wait(&Rand, 0xFFFF);
	ballParam->x = rand()%(MAXX-2*ballParam->radius);
	ballParam->y = rand()%(MAXY-2*ballParam->radius);
	ballParam->speed_x = (rand()%7) -3; // can only be from 0-6 then shifted to -3 to 3
	ballParam->speed_y = (rand()%7) -3;
	os_sem_send(&Rand);			
}

void button_pressed_callback( void ){
	//change what you want the function to do when the button is pressed
	create = true;
}

//check the position of the ball. if off the map re store its old value and change direction of the speed
void check_edge( bounceBall* ballParam, int old_x, int old_y ){
	if((ballParam->y < 0) || (ballParam->y > (MAXY - 2*ballParam->radius))){
		ballParam->y = old_y;
		ballParam->speed_y = -(ballParam->speed_y);
	}
	if((ballParam->x < 0) || (ballParam->x > (MAXX - 2*ballParam->radius))){
		ballParam->x = old_x;
		ballParam->speed_x = -(ballParam->speed_x);
	}
}

void draw_ball( bounceBall* ballParam, int old_x, int old_y, unsigned char* bitmap, unsigned char* clearbitmap ){
	os_sem_wait(&Draw, 0xFFFF);
 	GLCD_Bitmap (old_x, old_y, 2*ballParam->radius, 2*ballParam->radius, (unsigned char*)clearbitmap);
 	GLCD_Bitmap (ballParam->x, ballParam->y, 2*ballParam->radius, 2*ballParam->radius, (unsigned char*)bitmap);
	
	//DrawBall (old_x, old_y, ballParam->size_x, ballParam->size_y, (unsigned char*)clearbitmap, BG, BG);
 	//DrawBall (ballParam->x, ballParam->y, ballParam->size_x, ballParam->size_y, (unsigned char*)bitmap, BG, ballParam->color);
//  	ClearCircle (old_x, old_y, ballParam->size_x, ballParam->size_y, (unsigned char*)bitmap, White);
//  	DrawCircle (ballParam->x, ballParam->y, ballParam->size_x, ballParam->size_y, (unsigned char*)bitmap, BG, ballParam->color);
	os_sem_send(&Draw);
}

//clear the old location and draw the ball using "dx*dx + dy*dy <= r*r" (distance equation because the distance to the center should be less than radius
void draw_ball2( int x, int y, int old_x, int old_y, int r, unsigned short colour ){
	int i, j;
	double dx, dy;
	os_sem_wait(&Draw, 0xFFFF);
	// clear old circle
	GLCD_SetTextColor(BG);
	//DrawCircle(old_x, old_y, r);
	for (i = old_x-r; i<old_x+r; i++){
		for (j = old_y-r; j<old_y+r; j++){
			dx = i-old_x;
			dy = j-old_y;
			if((dx*dx + dy*dy) <= r*r)
				GLCD_PutPixel(i, j);
		}
	}
	
	//draw circle
	GLCD_SetTextColor(colour);
	for (i = x - r; i < x+r; i++){
		for (j = y - r; j < y+r; j++){
			dx = i-x;
			dy = j-y;
			if((dx*dx + dy*dy) <= r*r)
				GLCD_PutPixel(i, j);
		}
	}
	//DrawCircle(x, y, r);
	os_sem_send(&Draw);
}


unsigned short getColour( void ){
	return (colourList[rand()%15]);
}

//set random size, colour, inital position, speed and ...
void setRandom( bounceBall* ball ){
		int i, j;
		int x,y;	
		double dx, dy;
		int radius;
		int x0 = ball->x;
		int y0 = ball->y;

		ball->active = true;
		ball->radius = (rand()%5+2);
		position(ball);
		ball->color = getColour();
		radius = ball->radius;
// 		switch(rand()%3){
// 			case 0:
// 				ball->bitmap = (unsigned char*)smallBall;
// 				ball->clearbitmap = (unsigned char*)smallBallclear;
// // 				ball->size_x = 10;
// // 				ball->size_y = 10;
// 				ball->radius = 5;
// 			break;
// 			case 1:
// 				ball->bitmap = (unsigned char*)mediumBall;
// 				ball->clearbitmap = (unsigned char*)mediumBallclear;
// // 				ball->size_x = 14;
// // 				ball->size_y = 14;
// 				ball->radius = 7;
// 			break;
// 			default:
// 				ball->bitmap = (unsigned char*)largeBall;
// 				ball->clearbitmap = (unsigned char*)largeBallclear;
// // 				ball->size_x = 20;
// // 				ball->size_y = 20;
// 				ball->radius = 10;
// 			break;
// 		}
	
// 		for(y=-ball->radius; y<=ball->radius; y++){
// 			for(x=-ball->radius; x<=ball->radius; x++){
//         if(x*x+y*y <= ball->radius*ball->radius){
//            ball->bitmap[ball->radius*2*(x+ball->radius)+(y+ball->radius)]= (unsigned short*)PU;
// 				}
// 				else{
// 						ball->bitmap[ball->radius*2*(x+ball->radius)+(y+ball->radius)]= (unsigned short*)White;
// 				}
// 			}
// 		}	

// 	// REWRITE THIS DOESNT WORK AS WELL
// 	for (y=0; y<=2*radius; y++) {
// 		for (x=0; x<=2*radius; x++) {
// 			int index = (x * 2*radius) + y;
// 			if ((y-radius)*(y-radius)+(x-radius)*(x-radius) <= radius*radius) {
// 				ball->bitmap[index] = ball->color;
// 			}
// 			else
// 			{
// 				ball->bitmap[index] = White;
// 			}
// 		}
// 	}

		for (i = (2*ball->radius-1)*2*ball->radius; i > -1; i --) {
			for (j = 0; j < 2*ball->radius; j++) {
				dx = i-ball->radius;
				dy = j-ball->radius;
				if(dx*dx+dy*dy <= ball->radius*ball->radius)
					ball->bitmap[i+j] = PU;//ball->color;
				else
					ball->bitmap[i+j] = BG;
			}
		}
	
}
 
// ################### TASKS ###################
__task void ball_task( void* ballParam ){
	int old_x = 0, old_y = 0;
	bounceBall* ball = (bounceBall*)ballParam;						
	
	while(ball->active){
		os_sem_wait(&Draw, 0xFFFF);
		GLCD_Bitmap (old_x, old_y, 2*ball->radius, 2*ball->radius, (unsigned char*)ball->clearbitmap);
		os_sem_send(&Draw);

		//move the ball
		ball->x += ((double)ADCValue()/MAX_ADC)*ball->speed_x;
		ball->y += ((double)ADCValue()/MAX_ADC)*ball->speed_y;
		check_edge(ball, old_x, old_y);
		
		//draw the ball
// 		draw_ball(ball, old_x, old_y,(unsigned char*) ball->bitmap,(unsigned char*) ball->clearbitmap);
// 		draw_ball(ball->x+(ball->radius), ball->y+(ball->radius), old_x+(ball->radius), old_y+(ball->radius), ((ball->radius+ball->radius)), ball->color);
		os_sem_wait(&Draw, 0xFFFF);
		GLCD_Bitmap (ball->x, ball->y, 2*ball->radius, 2*ball->radius, (unsigned char*)ball->bitmap);
		os_sem_send(&Draw);
		
		//assign the new value to the old and continue
		old_x = ball->x;
		old_y = ball->y;
	}
	//clear the ball when it dies
   os_sem_wait(&Draw, 0xFFFF);
// 	//ClearCircle (old_x, old_y, ball->size_x, ball->size_y, (unsigned char*)ball->bitmap, BG);
 	GLCD_Bitmap (old_x, old_y, 2*ball->radius, 2*ball->radius, (unsigned char*)ball->clearbitmap);
 	os_sem_send(&Draw);

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

__task void checkCollision( void ) {
	int i, j, dx, dy, r;
	while(1) {
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
						balls -=2;

						if(ballParam[userBall]->active == false){
							//change user ball to the first active ball
							userBall = 0;
							while(!ballParam[userBall]->active && userBall < MAX_BALLS - 1)
								userBall++; //iterate until an active ball is found
						}
					}
				}
			}
		}
		os_sem_send(&Collide);
	}
}

__task void init_task( void ) {
  int debouncecount = 0, firstfree = 0;
	bool debounced = true;
	// Increase the priority to intilize the task first, then allow them to start
	os_tsk_prio_self ( 2 );
		
	//initialize semaphores
	os_sem_init(&Draw, 1);
	os_sem_init(&Rand, 1);
	os_sem_init(&Create, 1);
	os_sem_init(&Collide, 1);
	
	//first ball is created below because "create" is initally set to true
	
	// Launch a multi thread checking system
	os_tsk_create (readPoti_task, 1);
	os_tsk_create (ballCountLED, 1);
	os_tsk_create (joyControl, 1);
	os_tsk_create (checkCollision, 1);
	
	os_tsk_prio_self ( 1 );
	
	while(1){
		if(!create){
			debouncecount++;
			if(debouncecount > 100){
				debounced = true;
				debouncecount = 0;
			}
		}else
			debouncecount =0;
			
		//create ball
		if(create && debounced ){
			debounced = false;
			if (balls < 8){
				firstfree = 0;
// 				while (ballParam[firstfree]->active && firstfree < 7)
// 					firstfree++;//loop through to find the first free ball
				
				os_sem_wait(&Create, 0xFFFF);
				create = false;
				userBall = firstfree;
				setRandom(ballParam[firstfree]);
				os_tsk_create_ex (ball_task, 1, (void*)ballParam[firstfree]);
				balls++;
				os_sem_send(&Create);
			}
			else {
				os_sem_wait(&Create, 0xFFFF);
				create = false;
				os_sem_send(&Create);
			}
		}
	}
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
	INT0Init(button_pressed_callback);
	ADCInit();
	KBD_Init();
	
	//allocation all the ball structs and deactivate it
	for( i = 0; i < 8; i++){
		ballParam[i] = (bounceBall*)malloc(sizeof(bounceBall));
		ballParam[i]->active = false;
 		//ballParam[i]->bitmap = (unsigned char*) malloc(sizeof(unsigned short));
 		ballParam[i]->clearbitmap = (unsigned char*)largeBallclear;
	}

	os_sys_init(init_task);
}
