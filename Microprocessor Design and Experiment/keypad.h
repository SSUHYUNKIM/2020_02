#include <avr/io.h>      

#ifndef  F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>

void key_init(void);
unsigned char keypad_keypress(void);
