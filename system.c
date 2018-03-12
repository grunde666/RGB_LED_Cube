/*
 * Includes system specific functions like pin assignment
 */
 #include "system.h"
 #include <avr/io.h>

void intToASCII(uint8_t src, char* dest)
{
    uint8_t h_src = 0;

    h_src = src/100;
    dest[0] = h_src + 48;

    src -= h_src * 100;
    h_src = src/10;
    dest[1] = h_src + 48;

    src -= h_src * 10;
    dest[2] = src + 48;
}
