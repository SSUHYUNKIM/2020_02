#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "clcd.h"
#include "uart.h"
#include "keypad.h"
 
#ifndef  F_CPU
#define F_CPU 16000000UL
#endif
 
int set_rand(void);
int set_difficulty(void);
void baseball_game(void);
