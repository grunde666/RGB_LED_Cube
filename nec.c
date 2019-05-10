#include "nec.h"
#include "system.h"
#include "timer.h"
#include "uart.h"
#include <avr/interrupt.h>
#include <stdbool.h>

#define MARK 0
#define SPACE 1
#define GAP_TICKS 20
#define REPEAT 0xFFFFFFFF

#define NEC_BITS          32
#define NEC_HDR_MARK    9000
#define NEC_HDR_SPACE   4500
#define NEC_BIT_MARK     560
#define NEC_ONE_SPACE   1690
#define NEC_ZERO_SPACE   560
#define NEC_RPT_SPACE   2250

#define NEC_SENSE_PIN   PB4
#define NEC_SENSE_IREG  PINB
#define NEC_SENSE_OREG  PORTB
#define NEC_SENSE_DREG  DDRB

struct irRemote {
    uint8_t rawbuffer[100];
    uint8_t rawlen;
    uint8_t timer;
    uint8_t overflow;
    uint8_t rxState;
};

enum irStates {
    NEC_IR_STATE_IDLE,
    NEC_IR_STATE_MARK,
    NEC_IR_STATE_SPACE,
    NEC_IR_STATE_STOP,
    NEC_IR_STATE_OVERFLOW
};

static volatile struct irRemote irRemoteData;

void NEC_Init(void)
{
    /* Configure pin as input, no pullup */
	NEC_SENSE_DREG &= ~(1 << NEC_SENSE_PIN);
	NEC_SENSE_OREG &= ~(1 << NEC_SENSE_PIN);
}

void NEC_CheckInput (void) {
    uint8_t irData = NEC_SENSE_IREG & (1 << NEC_SENSE_PIN);

    irRemoteData.timer++;

    if(irRemoteData.rawlen >= 100) {
        irRemoteData.rxState = NEC_IR_STATE_OVERFLOW;
    }

    switch(irRemoteData.rxState) {
    //......................................................................
    case NEC_IR_STATE_IDLE: // In the middle of a gap
        if (irData == MARK) {
            if (irRemoteData.timer < GAP_TICKS)  {  // Not big enough to be a gap.
                irRemoteData.timer = 0;

            } else {
                // Gap just ended; Record duration; Start recording transmission
                irRemoteData.overflow = false;
                irRemoteData.rawlen = 0;
                irRemoteData.rawbuffer[irRemoteData.rawlen++] = irRemoteData.timer;
                irRemoteData.timer = 0;
                irRemoteData.rxState = NEC_IR_STATE_MARK;
            }
        }
        break;
    //......................................................................
    case NEC_IR_STATE_MARK:  // Timing Mark
        if (irData == SPACE) {   // Mark ended; Record time
            irRemoteData.rawbuffer[irRemoteData.rawlen++] = irRemoteData.timer;
            irRemoteData.timer = 0;
            irRemoteData.rxState = NEC_IR_STATE_SPACE;
        }
        break;
    //......................................................................
    case NEC_IR_STATE_SPACE:  // Timing Space
        if (irData == MARK) {  // Space just ended; Record time
            irRemoteData.rawbuffer[irRemoteData.rawlen++] = irRemoteData.timer;
            irRemoteData.timer = 0;
            irRemoteData.rxState = NEC_IR_STATE_MARK;

        } else if (irRemoteData.timer > GAP_TICKS) {  // Space
                // A long Space, indicates gap between codes
                // Flag the current code as ready for processing
                // Switch to STOP
                // Don't reset timer; keep counting Space width
                irRemoteData.rxState = NEC_IR_STATE_STOP;
        }
        break;
    //......................................................................
    case NEC_IR_STATE_STOP:  // Waiting; Measuring Gap
        if (irData == MARK)  irRemoteData.timer = 0 ;  // Reset gap timer
        break;
    //......................................................................
    case NEC_IR_STATE_OVERFLOW:  // Flag up a read overflow; Stop the State Machine
        irRemoteData.overflow = true;
        irRemoteData.rxState = NEC_IR_STATE_STOP;
        break;
    }
}

void NEC_CheckData (void){

}

uint32_t NEC_decode(void) {
    uint32_t  data   = 0;  // We decode into here; Start with nothing
    uint8_t   offset = 1;  // Index into results; Skip first entry!?

    if((irRemoteData.rxState == NEC_IR_STATE_STOP) && (irRemoteData.overflow == false)) {
        // Check header "mark"
        if(MATCH(irRemoteData.rawbuffer[offset]*256, NEC_HDR_MARK)) {
            offset++;

            // Check for repeat
            if ((irRemoteData.rawlen == 4)
                && (MATCH(irRemoteData.rawbuffer[offset]*256, NEC_RPT_SPACE)
                && (MATCH(irRemoteData.rawbuffer[offset]*256, NEC_BIT_MARK)
            {
                data = REPEAT;
            }
            // Check we have enough data
            else if (irRemoteData.rawlen < (2 * NEC_BITS) + 4) {
                data = 0;
            }
            // Check header "space"
            else if (!(MATCH(irRemoteData.rawbuffer[offset]*256, NEC_HDR_SPACE))) {
                data = 0;
            }
            else {
                offset++;

                // Build the data
                for (int i = 0;  i < NEC_BITS;  i++) {
                    // Check data "mark"
                    if (!(MATCH(irRemoteData.rawbuffer[offset]*256, NEC_BIT_MARK))) {
                        data = 0;
                        break;
                    }
                    offset++;
                    // Suppend this bit
                    if (MATCH(irRemoteData.rawbuffer[offset]*256, NEC_ONE_SPACE)) {
                        data = (data << 1) | 1 ;
                    }
                    else if (MATCH(irRemoteData.rawbuffer[offset]*256, NEC_ZERO_SPACE)) {
                        data = (data << 1) | 0 ;
                    }
                    else {
                        data = 0;
                        break;
                    }
                    offset++;
                }


            }
        }
        irRemoteData.rxState = NEC_IR_STATE_IDLE;
        irRemoteData.rawlen = 0;
    }
    return data;
}

uint8_t MATCH(uint16_t measured_ticks, uint16_t desired_us) {
    uint8_t passed = 0;

    if (measured_ticks > (desired_us - 256) && (measured_ticks < (desired_us + 256)) {
        passed = 1;
    }

    return passed;
}
