#include "timer.h"
#include "rc5.h"
#include "tlc5940.h"
#include "animations.h"
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t rc5_trigger;
volatile uint8_t msCount;
volatile uint16_t gs_cnt;

/* Timer 0 - Frame Updater */
void Timer0_Init(void)
{
    // ~4kHz for sampling of IR sensor
    // ~2kHz for update of led driver data
    // ((16000000/256)/3906) = 16
    TCCR0 |= (1 << WGM01); //CTC mode
    OCR0 = 16 - 1;
    TIMSK |= (1 << OCIE0);
    TCCR0 |= (1 << CS02) | (1 << CS00); // prescaler = 256
}

/* Timer 2 - GSCLK */
void Timer2_Init(void)
{
    TCCR2 = (1 << COM20)     // set on BOTTOM, clear on OCR2 (non-inverting),
           | (1 << WGM21);   // output on OC2, CTC mode with OCR2 top
    OCR2 = TLC_GSCLK_PERIOD; // see tlc_config.h
    TCCR2 |= (1 << CS20);    // no prescale, (start pwm output)
    TCCR1B |= (1 << CS10);  // no prescale, (start pwm output)
    TCCR0 |= (1 << CS02) | (1 << CS00); // prescaler = 256
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

