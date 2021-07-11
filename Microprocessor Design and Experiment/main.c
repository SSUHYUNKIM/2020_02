#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "clcd.h"
#include "uart.h"
#include "keypad.h"

#ifndef  F_CPU
#define F_CPU 16000000UL
#endif

int main(void)
{
	uart0_init();
	i2c_lcd_init();

	int answer = rand()%10000; //random answer 0000~9999
	char answer_num[4] = "0000"; //store answer
	itoa(answer, answer_num, 10);
	for(int i=3; i>=0; i--) {
		if(answer_num[i] == '\n')
			answer_num[i] = '0'; //if (answer <10000) put zero 
	}
	i2c_lcd_init();
	//i2c_lcd_string(0, 0, answer_num); //for test
	//_delay_ms(500);

	while(1) {
	char str0[20] = "Press Key";
	char str[30] = ""; //store pressed key num
	char mynum[30] = "";
	int strike = 0, ball = 0;
	char strike_string[10] = ""; //store strike
	char ball_string[10] = ""; //store ball


	
	for(int i = 0; i< 100; i++) {
		char temp[10] = "";
		key_init();
		i2c_lcd_init();
		i2c_lcd_string(0, 0, str0);
		int key_value = keypad_keypress(); //press key
		_delay_ms(500);
		
		if(key_value == 13) { //if press '13' erase one number
			int j = 0;	
			str[j-1] = '\0';
		}
		
		itoa(key_value, temp, 10);
		strcat(str, temp);
		}
		i2c_lcd_init();
		i2c_lcd_string(0, 0, str0);
		i2c_lcd_string(1, 0, str); //print you input in LCD
		_delay_ms(1000);
			
	}
	i2c_lcd_init();
	i2c_lcd_string(0,0,"your num is: ");
	i2c_lcd_string(1, 0, mynum); //print your final input in LCD
	
	
	itoa(strike, strike_string, 10); //print strike and ball
	itoa(ball, ball_string, 10);
	i2c_lcd_init();
	i2c_lcd_string(0,0, strike_string);
	i2c_lcd_string(0, 2, "Strike");
	i2c_lcd_string(1, 0, ball_string);
	i2c_lcd_string(1, 2, "Ball");
	_delay_ms(2000);
	
	if(strike == 4) { //if input and answer is totally correct, player wins
		i2c_lcd_init();
		i2c_lcd_string(0, 0, "You Win!");
		_delay_ms(2000);
		break;
	}
	else {
		i2c_lcd_init();
		i2c_lcd_string(0, 0, "Try Again...");
		_delay_ms(2000);
	}

	}
}
