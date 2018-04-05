#include "rc5.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t rc5_trigger;
volatile uint8_t msCount;

void Timer2_Init(void)
{
    // Initialisierung:
    TCCR2 = (1<<CS22);		// Prescaler von 64 und damit Timer starten
    TCNT2  = 5;			// Vorladen für 1 ms
    TIMSK |= (1<<TOIE2);	// Interrupts aktivieren
    sei();
}

ISR(TIMER2_OVF_vect)
{
    if(msCount<50)
    {
        msCount++;
    }
    TCNT2 = 5;		// Nachladen
}

