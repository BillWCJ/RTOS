#ifndef LED_H_
#define LED_H_

void LEDInit( void );

// Turn on the LED inn a position within 0..7
void turnOnLED( unsigned char led );

// Turn off the LED in the position within 0..7
void turnOffLED( unsigned char led );

#endif
