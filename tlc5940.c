#include "tlc5940.h"
#include <avr/io.h>

#define DCRPG_DREG  DDRC
#define DCRPG_PORT  PORTC
#define DCRPG_PIN   PC3

#define TLC5940_DREG    DDRD
#define TLC5940_PORT    PORTD
#define GSCLK_PIN       PD7
#define BLANK_PIN       PD6
#define XLAT_PIN        PD5
#define SCLK_PIN        PD4
#define SIN_PIN         PD3

void tlc5940_Init(void)
{
    TLC5940_DREG = GSCLK_PIN | BLANK_PIN | XLAT_PIN | SCLK_PIN | SIN_PIN;
    DCRPG_DREG = DCRPG_PIN;
}
