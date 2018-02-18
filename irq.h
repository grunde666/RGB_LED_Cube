#ifndef IRQ_H
#define IRQ_H

#include <avr/interrupt.h>

/* Saves current interrupt setting and disables interrupts globally */
inline void irq_lock(uint8_t *reg)
{
	*reg = SREG;
	cli();
}

/* Restores the interrupt setting previously saved to 'reg' */
inline void irq_unlock(uint8_t reg)
{
	SREG = reg;
}

inline void irq_enable(void)
{
	sei();
}

#endif
