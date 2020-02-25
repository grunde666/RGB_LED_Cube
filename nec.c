#include "nec.h"
#include "system.h"
#include "timer.h"
#include "uart.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

#define NEC_SENSE_PIN   PB4
#define NEC_SENSE_IREG  PINB
#define NEC_SENSE_OREG  PORTB
#define NEC_SENSE_DREG  DDRB

#define NEC_PULSE_NUMBER 32
#define NEC_PULSEWIDTH_ONE  4

#define KEYCODE_POWER           UINT32_C(0xF7C03F)
#define KEYCODE_WARM_WHITE      UINT32_C(0xF740BF)
#define KEYCODE_BRIGHTNESS_UP   UINT32_C(0xF700FF)
#define KEYCODE_BRIGHTNESS_DOWN UINT32_C(0xF7807F)
#define KEYCODE_PLAY_DEMO       UINT32_C(0xF7E01F)
#define KEYCODE_COLOR_CHANGE    UINT32_C(0xF710EF)
#define KEYCODE_FADE            UINT32_C(0xF7C837)

volatile unsigned long int keyCode = 0;

void NEC_Init(void)
{
    /* Configure pin as input, no pullup */
	NEC_SENSE_DREG &= ~(1 << NEC_SENSE_PIN);
	NEC_SENSE_OREG &= ~(1 << NEC_SENSE_PIN);
}

void NEC_CheckInput(void)
{
    static uint8_t timerCNT;
    static int8_t pulseCount;
    static unsigned long bitPattern;
    static uint8_t pinState = 1;

    keyCode = 0;
    timerCNT++;

    if((NEC_SENSE_IREG & (1 << NEC_SENSE_PIN)) && pinState == 0) { //Rising edge detection
        pinState = 1;
    }
    else if(!(NEC_SENSE_IREG & (1 << NEC_SENSE_PIN)) && pinState == 1) { //Falling edge detection
        pinState = 0;
        pulseCount++;

        if((timerCNT >= 25)) { // If the pulse width is greater than 12 ms, this will mark the SOF
            pulseCount = -1; // First count needs to be skipped hence pulse count is set to -1
            bitPattern = 0;
        }
        else if((pulseCount >= 0) && (pulseCount < NEC_PULSE_NUMBER)) { //Accumulate the bit values between 0-31.
            if(timerCNT >= NEC_PULSEWIDTH_ONE) { //pulse width greater than 2ms is considered as LOGIC1
                bitPattern |=(unsigned long)1<<(31-pulseCount);
            }

            if(pulseCount == NEC_PULSE_NUMBER-1) { //This will mark the End of frame as 32 pulses are received
                keyCode = bitPattern;                // Copy the newKey(patter) and set the pulse count to 0;
                pulseCount = 0;
            }
        }

        timerCNT = 0;   //Reset timer after analising pulse
    }
}

buttonID_t checkRemoteControlKey(unsigned long int keyCode)
{
    buttonID_t buttonID = 0;

    switch(keyCode) {
    case KEYCODE_POWER:
        //toggle power state of cube
        USART_puts("power button pressed!\n");
        buttonID = BUTTON_ID_POWER;
        break;
    case KEYCODE_WARM_WHITE:
        //change color to white --> stop demo
        USART_puts("warm-white button pressed!\n");
        buttonID = BUTTON_ID_WARM_WHITE;
        break;
    case KEYCODE_BRIGHTNESS_UP:
        //increase brightness until limit is reached
        USART_puts("brightness-up button pressed!\n");
        buttonID = BUTTON_ID_BRIGHTNESS_UP;
        break;
    case KEYCODE_BRIGHTNESS_DOWN:
        //decrease brightness until limit is reached
        USART_puts("brightness-down button pressed!\n");
        buttonID = BUTTON_ID_BRIGHTNESS_DOWN;
        break;
    case KEYCODE_COLOR_CHANGE:
        //increment index of color table --> stop demo
        USART_puts("color button pressed!\n");
        buttonID = BUTTON_ID_COLOR_CHANGE;
        break;
    case KEYCODE_PLAY_DEMO:
        //start play demo
        USART_puts("demo button pressed!\n");
        buttonID = BUTTON_ID_PLAY_DEMO;
        break;
    case KEYCODE_FADE:
        //start fading cube
        USART_puts("fade button pressed!\n");
        buttonID = BUTTON_ID_FADE;
        break;
    }

    return buttonID;
}
