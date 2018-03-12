#include "system.h"
#include "rc5.h"
#include "timer.h"
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define DEBUG

int main()
{
    struct rc5_data rc5_buf;
#ifdef DEBUG
    char debug_str[3];
    USART_Init();
#endif
//    rc5_init();
    Timer2_Init();
    USART_puts("start application...\n");

    while(1)
    {
//        if(rc5_recv(&rc5_buf))
//        {
//            intToASCII(rc5_buf.address, &debug_str[0]);
//            USART_puts(&debug_str[0]);
//            USART_putc('\t');
//
//            intToASCII(rc5_buf.command, &debug_str[0]);
//            USART_puts(&debug_str[0]);
//            USART_putc('\n');
//        }
    }

    return 0;
}
