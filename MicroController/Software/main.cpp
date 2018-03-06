#define F_CPU 2000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0

int main() {
	// Hello world!
	PORTA.DIRSET = (OUTPUT << 0);
	
	while (1) {
		PORTA.OUTSET |= (HIGH << 0);
		_delay_ms(1000);
		PORTA.OUTCLR |= (HIGH << 0);
		_delay_ms(1000);
	}
	
}

