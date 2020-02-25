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
#include "nec.h"
#include "timer.h"
#include "uart.h"
#include "tlc5940.h"
#include "animations.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

#include "transformation.h"
#include "definitions.h"
#include "animations.h"

#define BRIGHTNESS_STEP (255/15)

static void playDemo(void);

int main()
{
    unsigned long int keyCode = 0;
    uint8_t currentColor = HSV_COLOR_MAGENTA;
#ifdef DEBUG
    char debug_str[20]="";
    USART_Init();
#endif
    InitLayerPins();

    NEC_Init();
    InitADC();
    Timer0_Init();
    Tlc5940_Init();
    sei();
    USART_puts("start application...\n");

    mainState_t actualProgram = MAIN_STATE_PLAY_DEMO;
    mainState_t saveActualProgram = MAIN_STATE_PLAY_DEMO;

    globalHSV.h = color_table[currentColor].h;
    globalHSV.s = color_table[currentColor].s;
    globalHSV.v = color_table[currentColor].v;

    while(1) {
        if(frameReady == 0) {
            switch(actualProgram) {
            case MAIN_STATE_PLAY_DEMO:
                playDemo();
                break;
            case MAIN_STATE_MONOCHROME_CUBE:
                fillLEDCube(&globalHSV);
                break;
            case MAIN_STATE_POWER_DOWN:
                break;
            case MAIN_STATE_RGB_FADING:
                fadeColorCube(0);
                break;
            }
        }

        if(keyCode != 0) {
            sprintf(debug_str, "%08lx", keyCode);
            USART_puts("0x");
            USART_puts(debug_str);
            USART_putc('\n');

            buttonID_t buttonID;
            buttonID = checkRemoteControlKey(keyCode);

            switch(buttonID) {
            case BUTTON_ID_PLAY_DEMO:
                actualProgram = MAIN_STATE_PLAY_DEMO;
                break;
            case BUTTON_ID_POWER:
                if(actualProgram != MAIN_STATE_POWER_DOWN) {
                    saveActualProgram = actualProgram;
                    actualProgram = MAIN_STATE_POWER_DOWN;
                }
                else {
                    actualProgram = saveActualProgram;
                }
                break;
            case BUTTON_ID_COLOR_CHANGE:
                break;
            case BUTTON_ID_WARM_WHITE:
                actualProgram = MAIN_STATE_MONOCHROME_CUBE;
                currentColor = HSV_COLOR_WHITE;
                globalHSV.h = color_table[currentColor].h;
                globalHSV.s = color_table[currentColor].s;
                globalHSV.v = color_table[currentColor].v;
                break;
            case BUTTON_ID_FADE:
                actualProgram = MAIN_STATE_RGB_FADING;
                break;
            case BUTTON_ID_BRIGHTNESS_DOWN:
                if(globalHSV.v < BRIGHTNESS_STEP) {
                    globalHSV.v = 0;
                }
                else {
                    globalHSV.v -= BRIGHTNESS_STEP;
                }
                break;
            case BUTTON_ID_BRIGHTNESS_UP:
                if(globalHSV.v > (color_table[currentColor].v - BRIGHTNESS_STEP)) {
                    globalHSV.v = color_table[currentColor].v;
                }
                else {
                    globalHSV.v += BRIGHTNESS_STEP;
                }
                break;
            }
        }

        if(uart_str_complete)
        {
            if(uart_string[0] == 'c') {
                if(uart_string[1] == '+') {
                    if(currentColor == 12) {
                        currentColor = 0;
                    }
                    else {
                        currentColor++;
                    }
                    globalHSV.h = color_table[currentColor].h;
                    globalHSV.s = color_table[currentColor].s;
                }
                else if(uart_string[1] == '-') {
                    if(currentColor == 0) {
                        currentColor = 12;
                    }
                    else {
                        currentColor--;
                    }
                    globalHSV.h = color_table[currentColor].h;
                    globalHSV.s = color_table[currentColor].s;
                }
            }
            else if(uart_string[0] == 'h') {
                if(uart_string[1] == '-') {
                    globalHSV.v = globalHSV.v - 25;
                }
                else if(uart_string[1] == '+') {
                    globalHSV.v = globalHSV.v + 25;
                }
            }
            uart_str_complete = 0;
        }
    }

    return 0;
}

static void playDemo(void) {
    static uint8_t demoState = 0;
    switch(demoState) {
    case 0:
        if(fillCubeDiagonal(1) == 0) {
#ifdef DEBUG
//            USART_puts("fillCubeDiagonal finished\n");
#endif
            demoState = rand()%11;
//            demoState = 1;
        }
        break;
    case 1:
        if(rainfall(10) == 0) {
#ifdef DEBUG
//            USART_puts("rainfall finished\n");
#endif
            demoState = rand()%11;
//            demoState = 2;
        }
        break;
    case 2:
        if(activateRandomLED(65) == 0) {
            demoState = rand()%11;
#ifdef DEBUG
//            USART_puts("activateRandomLED finished\n");
#endif
//            demoState = 3;
        }
        break;
    case 3:
        if(fillCube_randomly(0) == 0) {
#ifdef DEBUG
//            USART_puts("fillCube_randomly finished\n");
#endif
            demoState = rand()%11;
//            demoState = 4;
        }
        break;
    case 4:
        if(clearCube_randomly(0) == 0) {
#ifdef DEBUG
//            USART_puts("clearCube_randomly finished\n");
#endif
            demoState = rand()%11;
//            demoState = 5;
        }
        break;
    case 5:
        if(dropLedTopDown(21) == 0) {
#ifdef DEBUG
//            USART_puts("dropLedTopDown finished\n");
#endif
            demoState = rand()%11;
//            demoState = 6;
        }
        break;
    case 6:
        if(fadeColorCube(0) == 0) {
#ifdef DEBUG
//            USART_puts("fadeColorCube finished\n");
#endif
            demoState = rand()%11;
//            demoState = 7;
        }
        break;
    case 7:
        if(floatingXLayer(3) == 0) {
#ifdef DEBUG
//            USART_puts("floatingXLayer finished\n");
#endif
            demoState = rand()%11;
//            demoState = 8;
        }
        break;
    case 8:
        if(floatingYLayer(3) == 0) {
#ifdef DEBUG
//            USART_puts("floatingYLayer finished\n");
#endif
            demoState = rand()%11;
//            demoState = 9;
        }
        break;
    case 9:
        if(floatingZLayer(3) == 0) {
#ifdef DEBUG
//            USART_puts("floatingZLayer finished\n");
#endif
            demoState = rand()%11;
//            demoState = 10;
        }
        break;
    case 10:
        if(randomLedColorCube(5) == 0) {
#ifdef DEBUG
//            USART_puts("randomLedColorCube finished\n");
#endif
            demoState = rand()%11;
//            demoState = 0;
        }
        break;
    }
}
