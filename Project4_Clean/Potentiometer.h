#ifndef POT_H_
#define POT_H_
#include <LPC17xx.h>
#include <RTL.h>
#include <stdio.h>
#include "led.h"

void ADCInit( void );

// Starting the conversion. Upon the call of this function, the ADC unit starts
// to read the connected port to its channel. The conversion takes 56 clock ticks.
// According the initialization, an intrupt will be called when the data becomes 
// ready.
void ADCConvert (void);

unsigned short int ADCValue( void );

#endif
