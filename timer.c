#include "timer.h"
#include "tlc5940.h"
#include "animations.h"
#include "nec.h"
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Timer 0 - Frame Updater */
void Timer0_Init(void)
{
    // ~4kHz for sampling of IR sensor
    // ~2kHz for update of led driver data
    // ((16000000/1024)/4) = 1953,125 Hz
    TCCR0 |= (1 << WGM01); //CTC mode
    OCR0 = 4 - 1;
    TIMSK |= (1 << OCIE0);
}

/* Timer 2 - GSCLK */
void Timer2_Init(void)
{
    TCCR2 = (1 << COM20)     // set on BOTTOM, clear on OCR2 (non-inverting),
           | (1 << WGM21);   // output on OC2, CTC mode with OCR2 top
    OCR2 = TLC_GSCLK_PERIOD; // see tlc_config.h
    TCCR2 |= (1 << CS20);    // no prescale, (start pwm output)
    TCCR1B |= (1 << CS10);  // no prescale, (start pwm output)
    TCCR0 |= (1 << CS02) | (1 << CS00); // prescaler = 1024
}

/* Timer 1 - BLANK / XLAT */
void Timer1_Init(void)
{
    TCCR1A = (1 << COM1B1); // non inverting, output on OC1B, BLANK
    TCCR1B = (1 << WGM13);  // Phase/freq correct PWM, ICR1 top
    OCR1A = 1;              // duty factor on OC1A, XLAT is inside BLANK
    OCR1B = 2;              // duty factor on BLANK (larger than OCR1A (XLAT))
    ICR1 = TLC_PWM_PERIOD;  // see tlc_config.h
}

ISR(TIMER1_OVF_vect)
{
    Tlc5940_interrupt();
}

ISR(TIMER0_COMP_vect)
{
    static uint8_t interruptState = 0;

    //Trigger check remote control input
    NEC_CheckInput();
    interruptState++;

    if(interruptState == 16) {
        //Trigger update layer
        updateLayerTriggerFlag = 1;
        interruptState = 0;
    }
}
