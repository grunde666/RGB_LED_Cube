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
#define NEC_PULSEWIDTH_ONE  7

volatile uint8_t necTriggerFlag = 0;

void NEC_Init(void)
{
    /* Configure pin as input, no pullup */
	NEC_SENSE_DREG &= ~(1 << NEC_SENSE_PIN);
	NEC_SENSE_OREG &= ~(1 << NEC_SENSE_PIN);
}

unsigned long NEC_CheckInput(void)
{
    static uint8_t timerCNT;
    static int8_t pulseCount;
    static unsigned long bitPattern;
    unsigned long newKey = 0;
    static uint8_t pinState = 1;

    timerCNT++;

    if((NEC_SENSE_IREG & (1 << NEC_SENSE_PIN)) && pinState == 0) //Rising edge detection
    {
        pinState = 1;
    }
    else if(!(NEC_SENSE_IREG & (1 << NEC_SENSE_PIN)) && pinState == 1) //Falling edge detection
    {
        pinState = 0;
        pulseCount++;

        if((timerCNT >= 50)) // If the pulse width is greater than 12 ms, this will mark the SOF
        {
            pulseCount = -1; // First count needs to be skipped hence pulse count is set to -1
            bitPattern = 0;
            USART_puts("S\n");
        }
        else if((pulseCount >= 0) && (pulseCount < NEC_PULSE_NUMBER)) //Accumulate the bit values between 0-31.
        {
            if(timerCNT >= NEC_PULSEWIDTH_ONE)  //pulse width greater than 2ms is considered as LOGIC1
            {
                bitPattern |=(unsigned long)1<<(31-pulseCount);
                USART_putc('1');
            }
            else
            {
                USART_putc('0');
            }
            USART_putc('\n');

            if(pulseCount == NEC_PULSE_NUMBER-1) //This will mark the End of frame as 32 pulses are received
            {
                newKey = bitPattern;                // Copy the newKey(patter) and set the pulse count to 0;
                pulseCount = 0;
            }
        }

        timerCNT = 0;   //Reset timer after analising pulse
    }

    return newKey;
}
