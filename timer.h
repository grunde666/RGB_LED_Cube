#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <avr/io.h>

/** Determines how long each PWM period should be, in clocks.
    \f$\displaystyle f_{PWM} = \frac{f_{osc}}{2 * TLC\_PWM\_PERIOD} Hz \f$
    \f$\displaystyle TLC\_PWM\_PERIOD = \frac{f_{osc}}{2 * f_{PWM}} \f$
    This is related to TLC_GSCLK_PERIOD:
    \f$\displaystyle TLC\_PWM\_PERIOD =
       \frac{(TLC\_GSCLK\_PERIOD + 1) * 4096}{2} \f$
    \note The default of 8192 means the PWM frequency is 976.5625Hz */
#define TLC_PWM_PERIOD    8192

/** Determines how long each period GSCLK is.
    This is related to TLC_PWM_PERIOD:
    \f$\displaystyle TLC\_GSCLK\_PERIOD =
       \frac{2 * TLC\_PWM\_PERIOD}{4096} - 1 \f$
    \note Default is 3 */
#define TLC_GSCLK_PERIOD    3

void Timer2_Init(void);
void Timer1_Init(void);
void Timer0_Init(void);

#endif // TIMER_H_INCLUDED
