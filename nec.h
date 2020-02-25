#ifndef NEC_H_INCLUDED
#define NEC_H_INCLUDED
#include "system.h"

typedef enum {
    BUTTON_ID_POWER,
    BUTTON_ID_BRIGHTNESS_UP,
    BUTTON_ID_BRIGHTNESS_DOWN,
    BUTTON_ID_COLOR_CHANGE,
    BUTTON_ID_FADE,
    BUTTON_ID_PLAY_DEMO,
    BUTTON_ID_WARM_WHITE
}buttonID_t;

void NEC_Init(void);
void NEC_CheckInput(void);
buttonID_t checkRemoteControlKey(unsigned long int newKeyCode);

extern volatile unsigned long int keyCode;

#endif // NEC_H_INCLUDED
