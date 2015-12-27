// #include <LPC17xx.h>
// #include <RTL.h>
// #include <stdio.h>
// #include "INT0.h"
// #include <stdbool.h>
// // Stop blinking
// // void (*Button_Pressed)(void) = NULL;
// volatile bool* pointer;

// /******************This section shows sample API to work with INT0 button******/

// void INT0Init(volatile bool* po) {

// 	// P2.10 is related to the INT0 or the push button.
// 	// P2.10 is selected for the GPIO 
// 	LPC_PINCON->PINSEL4 &= ~(3<<20); 

// 	// P2.10 is an input port
// 	LPC_GPIO2->FIODIR   &= ~(1<<10); 

// 	// P2.10 reads the falling edges to generate the IRQ
// 	// - falling edge of P2.10
// 	LPC_GPIOINT->IO2IntEnF |= (1 << 10);

// 	// IRQ is enabled in NVIC. The name is reserved and defined in `startup_LPC17xx.s'.
// 	// The name is used to implemet the interrupt handler above,
// 	NVIC_EnableIRQ( EINT3_IRQn );
// 	
// // 	//Set the call back function when button is pressed
// // 	Button_Pressed = button_pressed_callback;
// 	pointer = po; 

// }

// // INT0 interrupt handler
// void EINT3_IRQHandler( void ) {

// 	// Check whether the interrupt is called on the falling edge. GPIO Interrupt Status for Falling edge.
// 	if ( LPC_GPIOINT->IO2IntStatF && (0x01 << 10) ) {
// 		LPC_GPIOINT->IO2IntClr |= (1 << 10); // clear interrupt condition

// 		// Do the stuff 
// // 		Button_Pressed();
// 		*pointer = true;
// 	}
// }

