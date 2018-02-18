#include <avr/io.h>
#include <stdbool.h>

#include "disp.h"

#define DISP_NUM_LEDS 4

/*********************************************
 * Layout of LED segments and associated pins
 *
 * +-a-+     a = PC3   b = PC2
 * f   b     c = PC1   d = PC0
 * +-g-+     e = PB5   f = PC4
 * e   c     g = PC5
 * +-d-+
 *
 * Driver pins for LED display units
 *
 * LED3    LED2    LED1    LED0
 * PB4     PB3     PB2     PB1
 *********************************************/
#define DISP_PORTB_SHIFT  0
#define DISP_PORTC_SHIFT  8
#define DISP_PORTB_MASK   ((1 << PB1) | \
                           (1 << PB2) | \
                           (1 << PB3) | \
                           (1 << PB4) | \
                           (1 << PB5))
#define DISP_PORTC_MASK   ((1 << PC0) | \
                           (1 << PC1) | \
                           (1 << PC2) | \
                           (1 << PC3) | \
                           (1 << PC4) | \
                           (1 << PC5))

#define A  ((1u << DISP_PORTC_SHIFT) << PC3 )
#define B  ((1u << DISP_PORTC_SHIFT) << PC2 )
#define C  ((1u << DISP_PORTC_SHIFT) << PC1 )
#define D  ((1u << DISP_PORTC_SHIFT) << PC0 )
#define E  ((1u << DISP_PORTB_SHIFT) << PB5 )
#define F  ((1u << DISP_PORTC_SHIFT) << PC4 )
#define G  ((1u << DISP_PORTC_SHIFT) << PC5 )


const uint16_t disp_figure_table[] = {
	/* figure  segments */
	/* 0 */    A|B|C|D|E|F   ,
	/* 1 */    B|C           ,
	/* 2 */    A|B|G|E|D     ,
	/* 3 */    A|B|G|C|D     ,
	/* 4 */    F|G|B|C       ,
	/* 5 */    A|F|G|C|D     ,
	/* 6 */    A|F|E|D|C|G   ,
	/* 7 */    A|B|C         ,
	/* 8 */    A|B|C|D|E|F|G ,
	/* 9 */    A|B|C|D|F|G   ,
};
#define DISP_NUM_FIGURES  \
	(sizeof(disp_figure_table)/sizeof(disp_figure_table[0]))
 
const uint8_t disp_driver_mask[DISP_NUM_LEDS] = {
	/* LED0 */ (1 << PB1),
	/* LED1 */ (1 << PB2),
	/* LED2 */ (1 << PB3),
	/* LED3 */ (1 << PB4),
};

static uint16_t disp_buffer[DISP_NUM_LEDS];
static uint8_t disp_curr_led;

void disp_set_bcd(uint16_t data)
{
	uint8_t index;
	
	for (index = 0; index < DISP_NUM_LEDS; index++) {
		uint8_t bcd = data & 0xf;
		data >>= 4;
		
		if (bcd < DISP_NUM_FIGURES)
			disp_buffer[index] = disp_figure_table[bcd];
	}
}

void disp_tick(void)
{
	/* Advance to next segment, wrap over if needed */
	disp_curr_led++;
	if (disp_curr_led >= DISP_NUM_LEDS)
		disp_curr_led = 0;
		
	/* Clear all I/O pins owned by us */
	PORTB &= ~DISP_PORTB_MASK;
	PORTC &= ~DISP_PORTC_MASK;
	
	/* Set all segments of new pattern */
	PORTB |= (uint8_t) (disp_buffer[disp_curr_led] >> DISP_PORTB_SHIFT);
	PORTC |= (uint8_t) (disp_buffer[disp_curr_led] >> DISP_PORTC_SHIFT);
	
	/* Highlight current segment */
	PORTB |= disp_driver_mask[disp_curr_led];
}

void disp_init(void)
{
	disp_curr_led = 0;
	DDRB  |=  DISP_PORTB_MASK;
	PORTB &= ~DISP_PORTB_MASK;
	DDRC  |=  DISP_PORTC_MASK;
	PORTC &= ~DISP_PORTC_MASK;
}
