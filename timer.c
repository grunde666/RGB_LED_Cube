#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>
#include <stdbool.h>

#include "irq.h"

#include "rc5.h"
#include "main.h"
#include "disp.h"

#include "timer.h"

/* Runs every 100 microseconds from ISR context */
inline void timer_100us_isr(void)
{
	rc5_sample();
}

typedef void (*timer_proc_t)(void);

struct timer_entry {
	const timer_proc_t  proc;     // timer callback procedure
	const uint8_t       reload;   // reload value for counter
	uint8_t             counter;  // tick counter
	bool                ready;    // flag if ready to run
};

volatile struct timer_entry timer_table[] = {
	/* function   ticks */
	{  main_10ms,  100  },
	{  disp_tick,   20  },
};
#define TIMER_NUM_ENTRIES  (sizeof(timer_table)/sizeof(timer_table[0]))

///////////////////////////////////////////////////////////////////////

ISR(TIMER0_OVF_vect)
{
	volatile struct timer_entry *entry = timer_table;
	
	/* every 100 us */
	TCNT0 = (256 - 50);

	/* Call 100us first to reduce jitter */
	timer_100us_isr();
	
	while (entry != &timer_table[TIMER_NUM_ENTRIES]) {
		if (entry->counter > 0)
			entry->counter--;
		if (entry->counter == 0) {
			entry->counter = entry->reload;
			entry->ready = true;
		}
		entry++;
	}
}

void timer_dispatch(void)
{
	volatile struct timer_entry *entry = timer_table;

	while (entry != &timer_table[TIMER_NUM_ENTRIES]) {
		uint8_t lock;
		bool ready;
		
		irq_lock(&lock);
		ready = entry->ready;
		entry->ready = false;
		irq_unlock(lock);
		
		if (ready)
			entry->proc();
		entry++;
	}	
}

void timer_init(void)
{
	/* prescaler 1/8 */
	TCCR0 = (1 << CS01);
	/* enable interrupts */
	TIMSK |= (1 << TOIE0);
}
