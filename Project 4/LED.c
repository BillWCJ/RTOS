#include <LPC17xx.h>
#include <RTL.h>
#include <stdio.h>
#include "LED.h"

const unsigned char ledPosArray[8] = { 28, 29, 31, 2, 3, 4, 5, 6 };

/******************This section shows sample API to work with LEDs***********/

void LEDInit( void ) {

	// LPC_SC is a general system-control register block, and PCONP referes
	// to Power CONtrol for Peripherals.
	//  - Power/clock control bit for IOCON, GPIO, and GPIO interrupts (Section 4.8.9)
	//    This can also be enabled from `system_LPC17xx.c'
	LPC_SC->PCONP |= (1 << 15);            

	// The ports connected to p1.28, p1.29, and p1.31 are in mode 00 which
	// is functioning as GPIO (Section 8.5.5)
	LPC_PINCON->PINSEL3 &= ~(0xCF00);

	// The port connected to p2.2, p2.3, p2.4, p2.5, and p2.6 are in mode 00
	// which is functioning as GPIO (Section 8.5.5)
	LPC_PINCON->PINSEL4 &= (0xC00F);

	// LPC_GPIOx is the general control register for port x (Section 9.5)
	// FIODIR is Fast GPIO Port Direction control register. This register 
	// individually controls the direction of each port pin (Section 9.5)
	//
	// Set the LEDs connected to p1.28, p1.29, and p1.31 as output
	LPC_GPIO1->FIODIR |= 0xB0000000;           

	// Set the LEDs connected to p2.2, p2.3, p2.4, p2.5, and p2.6 as output port
	LPC_GPIO2->FIODIR |= 0x0000007C;           
}

// Turn on the LED inn a position within 0..7
void turnOnLED( unsigned char led ) {
	unsigned int mask = (1 << ledPosArray[led]);

	// The first two LEDs are connedted to the port 28, 29 and 30
	if ( led < 3 ) {
		// Fast Port Output Set register controls the state of output pins.
		// Writing 1s produces highs at the corresponding port pins. Writing 0s has no effect (Section 9.5)
		LPC_GPIO1->FIOSET |= mask;
	} else {
		LPC_GPIO2->FIOSET |= mask;
	}

}

// Turn off the LED in the position within 0..7
void turnOffLED( unsigned char led ) {
	unsigned int mask = (1 << ledPosArray[led]);

	// The first two LEDs are connedted to the port 28, 29 and 30
	if ( led < 3 ) {
		// Fast Port Output Clear register controls the state of output pins. 
		// Writing 1s produces lows at the corresponding port pins (Section 9.5)
		LPC_GPIO1->FIOCLR |= mask;
	} else {
		LPC_GPIO2->FIOCLR |= mask;
	}
}
