#include "rc5.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define RC5_SENSE_PIN   PB4
#define RC5_SENSE_IREG  PINB
#define RC5_SENSE_OREG  PORTB
#define RC5_SENSE_DREG  DDRB

/*
 * Each RC5 telegram has 14 bits:
 *
 *           |                   |                   |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 * | S1 | S2 | Tg | A4 | A3 | A2 | A1 | A0 | C5 | C4 | C3 | C2 | C1 | C0 |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 *
 * S1  Start bit, always 1
 * S2  Classic RC5: another start bit, 1
 *     Extended RC5: inverted command bit -C6
 * Tg  Toggle bit (inverted with each new key press)
 * Ax  Device address (0 to 31)
 * Cx  Command code (0 to 63, using -C6: 0 to 127)
 *
 * This implementation ignores the second start bit.
 */
static volatile uint16_t rc5_buffer;

#define RC5_BIT_S1  0x2000
#define RC5_BIT_S2  0x1000
#define RC5_BIT_TG  0x0800

struct rc5_bitfield
{
	uint16_t command : 6;
	uint16_t address : 5;
	uint16_t toggle  : 1;
	uint16_t start2  : 1;
	uint16_t start1  : 1;
	uint16_t unused  : 2;
};

bool rc5_recv(struct rc5_data *buf)
{
	uint8_t sreg;
	bool result = false;

    sreg = SREG;
    cli();
	const struct rc5_bitfield *data = (struct rc5_bitfield*) &rc5_buffer;
	if (data->start1)
	{
		buf->toggle = data->toggle;
		buf->address = data->address;
		buf->command = data->command;
		rc5_buffer = 0;
		result = true;
	}
    SREG = sreg;

	return result;
}

/*
 *  |  "1"  |  "1"  |  "0"  |  "1"  |  "1"  |  "0"  |
 *
 * -+---+   +---+       +---+---+   +---+       +---+-
 *      |   |   |       |       |   | ^ |       |
 *      +---+   +---+---+       +---+   +---+---+
 *                                            ^
 *      |---|   |-------|       |-----| |-----|
 *       min       max          sample  sample
 *
 * - The timer is synchronized with each edge in the middle of a bit.
 * - If time 'sample' elapsed the (next) bit is sampled.
 * - If an edge is detected before 'min' elapsed there is a violation of the
 *   protocol (e.g. a twitch caused by ambient light).
 *   Note: this test is not implemented because spurious impulses are caught
 *   by the 'max' timeout anway (because telegram is not completed).
 * - If an edge is detected after 'max' has been reached the telegram is
 *   either complete (if start1 is set) or crippled (start1 not set).
 */

#define RC5_TIME_MIN      6  // 600 us
#define RC5_TIME_SAMPLE  13  // 1,3 ms
#define RC5_TIME_MAX     26  // 2,6 ms

void rc5_sample(void)
{
	static uint8_t ticks;
	static bool old_state;
	const bool state = (RC5_SENSE_IREG & (1 << RC5_SENSE_PIN));

	if (state != old_state)
	{
		old_state = state;

		if (rc5_buffer == 0)
		{
			rc5_buffer = 1;
			ticks = 0;
		}

		if (ticks > RC5_TIME_SAMPLE)
			ticks = 0;
	}

	if (ticks == RC5_TIME_SAMPLE)
	{
		if ((rc5_buffer & RC5_BIT_S1) == 0)
		{
			rc5_buffer <<= 1;
			rc5_buffer |= state;
		}
	}

	if ((ticks > RC5_TIME_MAX) && ((rc5_buffer & RC5_BIT_S1) == 0))
	{
		rc5_buffer = 0;
	}

	if (ticks < 0xff)
		ticks++;
}

void rc5_init(void)
{
	/* Configure pin as input, no pullup */
	RC5_SENSE_DREG &= ~(1 << RC5_SENSE_PIN);
	RC5_SENSE_OREG &= ~(1 << RC5_SENSE_PIN);

	rc5_buffer = 0;
}
