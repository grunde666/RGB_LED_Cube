#ifndef NEC_H_INCLUDED
#define NEC_H_INCLUDED
#include "system.h"

void NEC_Init(void);
unsigned long NEC_CheckInput(void);
uint8_t checkRemoteControlKey(unsigned long int keyCode);

extern volatile uint8_t necTriggerFlag;

#endif // NEC_H_INCLUDED
