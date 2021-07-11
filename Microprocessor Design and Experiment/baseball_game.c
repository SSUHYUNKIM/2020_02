#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "clcd.h"
#include "keypad.h"
 
#ifndef  F_CPU
#define F_CPU 16000000UL
#endif
 
int set_rand(void) {
	char rand_num[3] = "000";
	i2c_lcd_init();
	i2c_lcd_string(0, 0, "Press Any Number");
	i2c_lcd_string(1, 0, "Ex: 005, 178");
	_delay_ms(2000);
	i2c_lcd_init();
	i2c_lcd_string(0, 0, "Press Key");
	_delay_ms(500);
	i2c_lcd_init();
	int i = 0;
	while(1) {
		char temp[1] = ""; 
		unsigned char key = keypad_keypress();
		if(key == 10) {
			key = 0;
		}
		if(key == 16) {
			return atoi(rand_num);
		}
 
		itoa(key, temp, 10);
		rand_num[2-i] = temp;
		i2c_lcd_string(0, i, temp);
		_delay_ms(1000);
		i++;
		if(i > 3) break;
	}
	return 0;
}
 
int set_difficulty(void) {
	i2c_lcd_init();
	i2c_lcd_string(0,0, "Set difficulty");
	i2c_lcd_string(1, 0, "1: Easy");
	i2c_lcd_string(2, 0, "2: Normal");
	i2c_lcd_string(3, 0, "3: Hard");
	_delay_ms(2000);
	i2c_lcd_init();
	i2c_lcd_string(0, 0, "Press Difficulty Key");
	unsigned char difficulty = keypad_keypress();
	if(difficulty == 1) {
		i2c_lcd_string(1, 0, "1: Easy");
		_delay_ms(1000);
		i2c_lcd_init();
		return 10000;
	}	
	else if(difficulty == 2) {
		i2c_lcd_string(1, 0, "2: Normal");
		_delay_ms(1000);
		i2c_lcd_init();
		return 10;
	}
	else if(difficulty == 3) {
		i2c_lcd_string(1, 0, "3: Hard");
		_delay_ms(1000);
		i2c_lcd_init();
		return 6;
	}
	return 0;
}
 
 
void baseball_game(void) {
	
	i2c_lcd_init();
	int answer;
	int rand_num = set_rand();
//test
	char a[10];
	itoa(rand_num, a, 10);
	i2c_lcd_string(0, 0, a);
	_delay_ms(500);
	i2c_lcd_init();
	srand(rand_num);
		answer = rand()%10000; //random answer 0000~9999
	
	
	char answer_num[4] = "0000"; //store answer
	itoa(answer, answer_num, 10);
	i2c_lcd_init();
	i2c_lcd_string(0, 0, "answer");
	i2c_lcd_string(1, 0, answer_num); //for test
	//_delay_ms(500);
	
	int difficulty = set_difficulty();
	int count = 0;
 
	while(1) {
	char str0[20] = "Press Your Input";
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
			while(str[j]) {
				j++;
			}
			str[j-1] = '\0';
		}
		else if(key_value == 16) { //if press '16' check input number
			strcpy(mynum, str);
			break;
		}
		else {
		if(key_value==10) { // if press '10' input number is '0'
			key_value = 0;
		}
		itoa(key_value, temp, 10);
		strcat(str, temp);
		}
		i2c_lcd_init();
		i2c_lcd_string(0, 0, str0);
		i2c_lcd_string(1, 0, str); //print you input in LCD
		_delay_ms(500);
			
	}
	i2c_lcd_init();
	i2c_lcd_string(0,0,"your num is: ");
	i2c_lcd_string(1, 0, mynum); //print your final input in LCD
	
	for(int i=0; i<4; i++) { //check input
		for(int j=0; j<4; j++) {
			if(mynum[j] == answer_num[i]) {
			if(i==j) {
				strike = strike + 1; //if it has perfectly same number and position, plus strike
				}
			else {
				ball = ball + 1; //if it has just same number(not same position), plus ball
			}
			}
 
		}
	}
	
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
	count++;
	if(count == difficulty) {
		i2c_lcd_init();
		i2c_lcd_string(0, 0, "To many tiral...");
		i2c_lcd_string(1, 0, "Sorry, You LOSE");
		_delay_ms(1000);
		break;
	}
	}
}
