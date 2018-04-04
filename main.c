#include "system.h"
#include "rc5.h"
#include "nec.h"
#include "timer.h"
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#define DEBUG

int main()
{
//    struct rc5_data rc5_buf;
    unsigned int keyCode;
#ifdef DEBUG
    char debug_str[12];
    USART_Init();
#endif
//    rc5_init();
    NEC_Init();
    Timer2_Init();
    USART_puts("start application...\n");

    while(1)
    {
        keyCode = NEC_CheckInput();

        if(keyCode != 0)
        {
            sprintf(debug_str, "%x", keyCode);
            USART_puts("0x");
            USART_puts(debug_str);
            USART_putc('\n');
        }
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
