#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

void USART_Init(void);
void USART_putc(unsigned char data);
void USART_puts (char *s);

#endif // UART_H_INCLUDED
