#include "nec.h"
#include "system.h"
#include "timer.h"
#include "uart.h"
#include <avr/interrupt.h>

#define NEC_SENSE_PIN   PB4
#define NEC_SENSE_IREG  PINB
#define NEC_SENSE_OREG  PORTB
#define NEC_SENSE_DREG  DDRB

#define NEC_PULSE_NUMBER 32
#define NEC_PULSEWIDTH_ONE  2

void NEC_Init(void)
{
    /* Configure pin as input, no pullup */
	NEC_SENSE_DREG &= ~(1 << NEC_SENSE_PIN);
	NEC_SENSE_OREG &= ~(1 << NEC_SENSE_PIN);
}

uint32_t NEC_CheckInput(void)
{
    static uint8_t timerValue;
    static int8_t pulseCount;
    static uint32_t bitPattern;
    uint32_t newKey = 0;
    static uint8_t pinState;

    //Falling edge detection
    if(!(NEC_SENSE_IREG & (1 << NEC_SENSE_PIN)) && pinState == 1)
    {
        pinState = 0;
        cli();
        timerValue = msCount;
        msCount = 0;
        sei();
        pulseCount++;

        if((timerValue>=50)) // If the pulse width is greater than 50ms, this will mark the SOF
        {
            pulseCount = -2; // First 2 counts needs to be skipped hence pulse count is set to -2
            bitPattern = 0;
        }
        else if((pulseCount>=0) && (pulseCount < NEC_PULSE_NUMBER)) //Accumulate the bit values between 0-31.
        {
            if(timerValue >= NEC_PULSEWIDTH_ONE)  //pulse width greater than 2ms is considered as LOGIC1
            {
                bitPattern |=(uint32_t)1<<(31-pulseCount);
                USART_putc('1');
            }
            else
            {
                 USART_putc('0');
            }
        }
        else if(pulseCount >= NEC_PULSE_NUMBER) //This will mark the End of frame as 32 pulses are received
        {
            newKey = bitPattern;                // Copy the newKey(patter) and set the pulse count to 0;
            pulseCount = 0;
        }
    }
    else if((NEC_SENSE_IREG & (1 << NEC_SENSE_PIN)) && pinState == 0)
    {
        pinState = 1;
    }

    return newKey;
}
