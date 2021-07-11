#ifndef UART_H_
#define UART_H_

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>

void uart0_init(void);
void uart0_rx_int_init(void);
void uart0_trasnmit(char data);
unsigned char uart0_receive(void);

#endif /* UART_H_ */
