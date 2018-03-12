#include "rc5.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t rc5_trigger;

void Timer2_Init(void)
{
    // Initialisierung:
    TCCR2 = (1<<CS22);		// Prescaler von 64 und damit Timer starten
    TCNT2  = 230;			// Vorladen
    TIMSK |= (1<<TOIE2);		// Interrupts aktivieren
    sei();
}

ISR(TIMER2_OVF_vect)
{
    TCNT2 = 230;		// Nachladen
    rc5_sample();
}
