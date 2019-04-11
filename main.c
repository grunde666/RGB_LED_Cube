/*
 * Device displays animations in different programs that user can choose
 * User can choose different colors during animations
 * User can control dimming value of cube during animations
 * User can control device with remote control
 * Different animation programs:
 *  - Demo: play pseudo random animations in different colors
 *  - wholeCube: enlight whole cube in user color (default color is green)
 *  - dimmingCube: dimm cube von 0% to 100% and vice versa
 *  - RGB dimming: going through all colors
*/

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
#include "definitions.h"
#include "animations.h"

static void playDemo(void);

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
    Timer0_Init();
    Tlc5940_Init();
    sei();
    USART_puts("start application...\n");

    uint8_t actualProgram = 0;

    globalHSV.h = color_table[HSV_COLOR_MAGENTA].h;
    globalHSV.s = color_table[HSV_COLOR_MAGENTA].s;
    globalHSV.v = color_table[HSV_COLOR_MAGENTA].v;

    while(1)
    {
        if(frameReady == 0) {
            switch(actualProgram) {
            case 0:
                playDemo();
                break;
            case 1:
    //            fillLEDCube(0,0);
                break;
            case 2:
    //            dimmingCube();
                break;
            case 3:
    //            playAllColors();
                break;
            }
        }

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

static void playDemo(void) {
    static uint8_t demoState = 0;
    static uint8_t colorCNT = 0;

    switch(demoState) {
    case 0:
        if(fillCubeDiagonal(1) == 0)
        {
#ifdef DEBUG
            USART_puts("rainfall\n");
#endif
            demoState = 1;
        }
        break;
    case 1:
        if(rainfall(10) == 0)
        {
#ifdef DEBUG
            USART_puts("activateRandomLED\n");
#endif
            demoState = 2;
        }
        break;
    case 2:
        if(activateRandomLED(10) == 0)
        {
#ifdef DEBUG
            USART_puts("fillCube_randomly\n");
#endif
            demoState = 3;
        }
        break;
    case 3:
        if(fillCube_randomly(0) == 0)
        {
#ifdef DEBUG
            USART_puts("clearCube_randomly\n");
#endif
            demoState = 4;
        }
        break;
    case 4:
        if(clearCube_randomly(0) == 0)
        {
#ifdef DEBUG
            USART_puts("dropLedTopDown\n");
#endif
            demoState = 5;
        }
        break;
    case 5:
        if(dropLedTopDown(10) == 0)
        {
#ifdef DEBUG
            USART_puts("fadeColorCube\n");
#endif
            demoState = 6;
        }
        break;
    case 6:
        if(fadeColorCube(0) == 0)
        {
#ifdef DEBUG
            USART_puts("floatingXLayer\n");
#endif
            demoState = 7;
        }
        break;
    case 7:
        if(floatingXLayer(3) == 0)
        {
#ifdef DEBUG
            USART_puts("floatingYLayer\n");
#endif
            demoState = 8;
        }
        break;
    case 8:
        if(floatingYLayer(3) == 0)
        {
#ifdef DEBUG
            USART_puts("floatingZLayer\n");
#endif
            demoState = 9;
        }
        break;
    case 9:
        if(floatingZLayer(3) == 0)
        {
#ifdef DEBUG
            USART_puts("randomLedColorCube\n");
#endif
            demoState = 10;
        }
        break;
    case 10:
        if(randomLedColorCube(5) == 0)
        {
#ifdef DEBUG
            USART_puts("different colors\n");
#endif
            demoState = 11;
        }
        break;
    case 11:
        if(fillCube_randomly(0) == 0)
        {
            if(colorCNT == 14) {
#ifdef DEBUG
                USART_puts("fillCubeDiagonal\n");
#endif
                demoState = 0;
                colorCNT = 0;
            }
            else {
                globalHSV.h = color_table[colorCNT].h;
                globalHSV.s = color_table[colorCNT].s;
                globalHSV.v = color_table[colorCNT].v;
                colorCNT++;
            }
        }
        break;
    }
}
