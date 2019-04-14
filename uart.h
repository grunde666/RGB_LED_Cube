#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

#include <avr/io.h>

#define UART_MAXSTRLEN 10

volatile uint8_t uart_str_complete ;
volatile uint8_t uart_str_count;
volatile char uart_string[UART_MAXSTRLEN + 1];

void USART_Init(void);
void USART_putc(unsigned char data);
void USART_puts (char *s);
void uart_gets( char* Buffer, uint8_t MaxLen );

#endif // UART_H_INCLUDED
