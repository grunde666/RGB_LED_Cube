#ifndef TLC5940_H_INCLUDED
#define TLC5940_H_INCLUDED

#include <avr/io.h>

#define DCRPG_DREG  DDRC
#define DCRPG_PORT  PORTC
#define DCRPG_PIN   PC3

#define TLC5940_DREG    DDRD
#define TLC5940_PORT    PORTD
#define GSCLK_PIN       PD7
#define SCLK_PIN        PD6
#define XLAT_PIN        PD5
#define BLANK_PIN       PD4
#define SIN_PIN         PD3

#define SetPin_SCLK()   TLC5940_PORT |= (1 << SCLK_PIN)
#define ClearPin_SCLK() TLC5940_PORT &= ~(1 << SCLK_PIN)

#define SetPin_SIN()    TLC5940_PORT |= (1 << SIN_PIN)
#define ClearPin_SIN()  TLC5940_PORT &= ~(1 << SIN_PIN)

#define SetPin_XLAT()   TLC5940_PORT |= (1 << XLAT_PIN)
#define ClearPin_XLAT() TLC5940_PORT &= ~(1 << XLAT_PIN)

#define NUM_TLCS 3

/** Enables the Timer1 Overflow interrupt, which will fire after an XLAT
    pulse */
#define set_XLAT_interrupt()    TIFR |= (1 << TOV1); TIMSK |= (1 << TOIE1)
/** Disables any Timer1 interrupts */
#define clear_XLAT_interrupt()  TIMSK &= ~(1 << TOIE1)

/** Enables the output of XLAT pulses */
#define enable_XLAT_pulses()    TCCR1A = (1 << COM1A1) | (1 << COM1B1)
/** Disables the output of XLAT pulses */
#define disable_XLAT_pulses()   TCCR1A = (1 << COM1B1)

/** If more than 16 TLCs are daisy-chained, the channel type has to be uint16_t.
    Default is uint8_t, which supports up to 16 TLCs. */
#define TLC_CHANNEL_TYPE    uint8_t

/* Various Macros */

/** Arranges 2 grayscale values (0 - 4095) in the packed array format (3 bytes).
    This is for array initializers only: the output is three comma seperated
    8-bit values. */
//#define GS_DUO(a, b)  ((a) >> 4), ((a) << 4) | ((b) >> 8), (b)
#define GS_DUO(a, b)    (uint8_t)((a) >> 4), (uint8_t)((a) << 4) | (uint8_t)((b) >> 8), (uint8_t)(b)

extern volatile uint8_t tlc_needXLAT;
//extern volatile void (*tlc_onUpdateFinished)(void);
extern uint8_t tlc_GSData[NUM_TLCS * 24];

void Tlc5940_Init(void);
void Tlc5940_clear(void);
uint8_t Tlc5940_update(void);
void Tlc5940_set(TLC_CHANNEL_TYPE channel, uint16_t value);
uint16_t Tlc5940_get(TLC_CHANNEL_TYPE channel);
void Tlc5940_setAll(uint16_t value);
void tlc_shift8_init(void);
void tlc_shift8(uint8_t byte);
void Tlc5940_interrupt(void);

#endif // TLC5940_H_INCLUDED
