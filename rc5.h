#ifndef RC5_H_INCLUDED
#define RC5_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

/* Data of one RC5 telegram */
struct rc5_data {
	uint8_t  address;  // device address
	uint8_t  command;  // command code
	bool     toggle;   // toggle bit
};

/* Get last received telegram.
 * Returns true if new data has arrived, otherwise false.
 */
bool rc5_recv(struct rc5_data *buf);
/* Callback function for sampling portpin,
 * must be called every 100 microseconds.
 */
void rc5_sample(void);
/* Initialization of RC5 module */
void rc5_init(void);

#endif // RC5_H_INCLUDED
