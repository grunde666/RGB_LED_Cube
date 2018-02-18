#ifndef DISP_H
#define DISP_H

#include <stdint.h>

/* Set display content from BCD data */
void disp_set_bcd(uint16_t data);
/* Tick for multiplexing */
void disp_tick(void);
/* Initialization of display */
void disp_init(void);

#endif
