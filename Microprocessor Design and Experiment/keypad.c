#include <avr/io.h>


#ifndef  F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>     

#define KEYPAD_PORT PORTA           //Keypad Port
#define KEYPAD_DDR DDRA         //Data Direction Register
#define KEYPAD_PIN PINA         //Keypad Port Pins


void key_init(){
        DDRA = 0x00; //4col: output, 4row: input
		DDRC = 0x0F;		
		PORTC = 0xFF;
}

unsigned char keypad_keypresss(void)
{
	DDRC = 0x0F;
	DDRA = 0x00;
    
    unsigned char key = 0x00, key_value=0x00, keypad_ax = 0x08;
    while(1) {
		keypad_ax = 0x01;
		for(int i = 0; i< 4; i++) {
			PORTC = keypad_ax;
			_delay_ms(500);
			key = PINA & 0x0F;
			
			if(key == 0x08) {
				key_value = 1 + i; break; 
			}
			else if(key == 0x04) {
				key_value = 5 + i; break;
			}
			else if(key == 0x02) {
				key_value = 9 + i; break;
			}
			else if(key == 0x01) {
				key_value = 13 + i; break;
			}
			keypad_ax = keypad_ax << 1;
		}
		if(key_value != 0x00) return key_value;
	}
	
}

