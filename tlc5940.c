#include "tlc5940.h"
#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/** This will be true (!= 0) if update was just called and the data has not
    been latched in yet. */
volatile uint8_t tlc_needXLAT;

/** Packed grayscale data, 24 bytes (16 * 12 bits) per TLC.

    Format: Lets assume we have 2 TLCs, A and B, daisy-chained with the SOUT of
    A going into the SIN of B.
    - byte 0: upper 8 bits of B.15
    - byte 1: lower 4 bits of B.15 and upper 4 bits of B.14
    - byte 2: lower 8 bits of B.0
    - ...
    - byte 24: upper 8 bits of A.15
    - byte 25: lower 4 bits of A.15 and upper 4 bits of A.14
    - ...
    - byte 47: lower 8 bits of A.0

    \note Normally packing data like this is bad practice.  But in this
          situation, shifting the data out is really fast because the format of
          the array is the same as the format of the TLC's serial interface. */
uint8_t tlc_GSData[NUM_TLCS * 24];

/** Interrupt called after an XLAT pulse to prevent more XLAT pulses. */
void Tlc5940_interrupt(void)
{
    disable_XLAT_pulses();
    clear_XLAT_interrupt();
    tlc_needXLAT = 0;
}

void Tlc5940_Init(void)
{
    DCRPG_DREG |= (1 << DCRPG_PIN);
//    DCRPG_PORT |= (1 << DCRPG_PIN);

    TLC5940_DREG |= (1 << GSCLK_PIN) | (1 << BLANK_PIN) | (1 << XLAT_PIN) | (1 << SCLK_PIN) | (1 << SIN_PIN);
    TLC5940_PORT |= (1 << BLANK_PIN);

    Tlc5940_setAll(0);
    Tlc5940_update();

    disable_XLAT_pulses();
    clear_XLAT_interrupt();
    tlc_needXLAT = 0;
    SetPin_XLAT();
    ClearPin_XLAT();
    Timer1_Init();
    Timer2_Init();
    Tlc5940_update();
}

/** \defgroup ReqVPRG_ENABLED Functions that Require VPRG_ENABLED
    Functions that require VPRG_ENABLED == 1.
    You can enable VPRG by changing
    \code #define VPRG_ENABLED    0 \endcode to
    \code #define VPRG_ENABLED    1 \endcode in tlc_config.h

    You will also have to connect Arduino digital pin 6 to TLC pin 27. (The
    pin to be used can be changed in tlc_config.h).  If VPRG is not enabled,
    the TLC pin should grounded (remember to unconnect TLC pin 27 from GND
    if you do enable VPRG). */
/* @{ */ /* @} */

/** \defgroup CoreFunctions Core Libary Functions
    These function are all prefixed with "Tlc." */
/* @{ */

/** Clears the grayscale data array, #tlc_GSData, but does not shift in any
    data.  This call should be followed by update() if you are turning off
    all the outputs. */
void Tlc5940_clear(void)
{
    Tlc5940_setAll(0);
}

/** Shifts in the data from the grayscale data array, #tlc_GSData.
    If data has already been shifted in this grayscale cycle, another call to
    update() will immediately return 1 without shifting in the new data.  To
    ensure that a call to update() does shift in new data, use
    \code while(Tlc.update()); \endcode
    or
    \code while(tlc_needXLAT); \endcode
    \returns 1 if there is data waiting to be latched, 0 if data was
             successfully shifted in */
uint8_t Tlc5940_update(void)
{
    if (tlc_needXLAT) {
        return 1;
    }
    disable_XLAT_pulses();
    SetPin_SCLK();
    ClearPin_SCLK();

    uint8_t *p = tlc_GSData;
    while (p < tlc_GSData + NUM_TLCS * 24) {
        tlc_shift8(*p++);
        tlc_shift8(*p++);
        tlc_shift8(*p++);
    }
    tlc_needXLAT = 1;
    enable_XLAT_pulses();
    set_XLAT_interrupt();
    return 0;
}

/** Sets channel to value in the grayscale data array, #tlc_GSData.
    \param channel (0 to #NUM_TLCS * 16 - 1).  OUT0 of the first TLC is
           channel 0, OUT0 of the next TLC is channel 16, etc.
    \param value (0-4095).  The grayscale value, 4095 is maximum.
    \see get */
void Tlc5940_set(TLC_CHANNEL_TYPE channel, uint16_t value)
{
    TLC_CHANNEL_TYPE index8 = (NUM_TLCS * 16 - 1) - channel;
    uint8_t *index12p = tlc_GSData + ((((uint16_t)index8) * 3) >> 1);
    if (index8 & 1) { // starts in the middle
                      // first 4 bits intact | 4 top bits of value
        *index12p = (*index12p & 0xF0) | (value >> 8);
                      // 8 lower bits of value
        *(++index12p) = value & 0xFF;
    } else { // starts clean
                      // 8 upper bits of value
        *(index12p++) = value >> 4;
                      // 4 lower bits of value | last 4 bits intact
        *index12p = ((uint8_t)(value << 4)) | (*index12p & 0xF);
    }
}

/** Gets the current grayscale value for a channel
    \param channel (0 to #NUM_TLCS * 16 - 1).  OUT0 of the first TLC is
           channel 0, OUT0 of the next TLC is channel 16, etc.
    \returns current grayscale value (0 - 4095) for channel
    \see set */
uint16_t Tlc5940_get(TLC_CHANNEL_TYPE channel)
{
    TLC_CHANNEL_TYPE index8 = (NUM_TLCS * 16 - 1) - channel;
    uint8_t *index12p = tlc_GSData + ((((uint16_t)index8) * 3) >> 1);
    return (index8 & 1)? // starts in the middle
            (((uint16_t)(*index12p & 15)) << 8) | // upper 4 bits
            *(index12p + 1)                       // lower 8 bits
        : // starts clean
            (((uint16_t)(*index12p)) << 4) | // upper 8 bits
            ((*(index12p + 1) & 0xF0) >> 4); // lower 4 bits
    // that's probably the ugliest ternary operator I've ever created.
}

/** Sets all channels to value.
    \param value grayscale value (0 - 4095) */
void Tlc5940_setAll(uint16_t value)
{
    uint8_t firstByte = value >> 4;
    uint8_t secondByte = (value << 4) | (value >> 8);
    uint8_t *p = tlc_GSData;
    while (p < tlc_GSData + NUM_TLCS * 24) {
        *p++ = firstByte;
        *p++ = secondByte;
        *p++ = (uint8_t)value;
    }
}

/* @} */

/** Shifts a byte out, MSB first */
void tlc_shift8(uint8_t byte)
{
    for (uint8_t bit = 0x80; bit; bit >>= 1) {
        if (bit & byte) {
            SetPin_SIN();
        } else {
            ClearPin_SIN();
        }
        SetPin_SCLK();
        ClearPin_SCLK();
    }
}
