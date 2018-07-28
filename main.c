#include "system.h"
#include "rc5.h"
#include "nec.h"
#include "timer.h"
#include "uart.h"
#include "tlc5940.h"
#include "animations.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "transformation.h"
//#define DEBUG

int main()
{
//    struct rc5_data rc5_buf;
//    uint32_t keyCode = 0;
//    unsigned int keyCode_low = 0;
//    unsigned int keyCode_high = 0;

#ifdef DEBUG
    char debug_str[12];
    USART_Init();
#endif
    InitLayerPins();

//    rc5_init();
//    NEC_Init();
//    Timer1_Init();
//    Timer2_Init();
    Timer0_Init();
    Tlc5940_Init();
    sei();
//    USART_puts("start application...\n");

//    Tlc5940_setAll(0);
//    uint8_t i;
//    for(i = 0; i < 16; i++)
//    {
//        if(i == 7)
//        {
//            Tlc5940_set(ledChannel_Array[i].b, 4095);
//            Tlc5940_set(ledChannel_Array[i].r, 4095);
//            Tlc5940_set(ledChannel_Array[i].g, 4095);
//        }
//        else
//        {
////            Tlc5940_set(ledChannel_Array[i].b, 1024);
////            Tlc5940_set(ledChannel_Array[i].r, 1024);
////            Tlc5940_set(ledChannel_Array[i].g, 1024);
//        }
//    }

//    while(Tlc5940_update()){;}

//    clearLEDCube();

//    PORTC &= ~((1 << LAYER_1_PIN) | (1 << LAYER_3_PIN) | (1 << LAYER_4_PIN));
//    PORTC |= (1 << LAYER_2_PIN);

    while(1)
    {
//        everyLED();
//        rain(10);
//        activateRandomLED(10);
//        fillCube_randomly(1);
//        clearCube_randomly(1);
//        dropLedTopDown(10);
//        floatingXLayer(3);
//        floatingYLayer(3);
//        floatingZLayer(3);
        blinkingCube(5);
//        fillLEDCube();
//        keyCode = NEC_CheckInput();
//
//        if(keyCode != 0)
//        {
//            keyCode_low = keyCode & 0xFFFF;
//            keyCode_high = keyCode >> 16;
//            sprintf(debug_str, "%x", keyCode_high);
//            USART_puts(" 0x");
//            USART_puts(debug_str);
//            sprintf(debug_str, "%x", keyCode_low);
//            USART_puts(debug_str);
//            USART_putc('\n');
//        }
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
