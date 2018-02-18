#include <stdint.h>

#include "irq.h"
#include "rc5.h"
#include "timer.h"
#include "disp.h"

void main_10ms(void)
{
	struct rc5_data telegram;

	/* Check if new telegram has been received */
	if (rc5_recv(&telegram)) {
		uint16_t data = 0;
		
		/* Construct display content from telegram data */
		data |= ((uint16_t)telegram.address / 10) << 12;
		data |= ((uint16_t)telegram.address % 10) <<  8;
		data |= ((uint16_t)telegram.command / 10) <<  4;
		data |= ((uint16_t)telegram.command % 10) <<  0;
		
		disp_set_bcd(data);
	}
}

int main(void)
{
	/* Initialize modules */
	rc5_init();
	timer_init();
	disp_init();

	/* Enable interrupts for timer */
	irq_enable();
	/* Clear display */
	disp_set_bcd(0xffff);
	
	/* Main loop to dispatch timer jobs */
	for (;;)
	{
		timer_dispatch();
	}
}
