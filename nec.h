#ifndef NEC_H_INCLUDED
#define NEC_H_INCLUDED
#include "system.h"

void NEC_Init(void);
unsigned long NEC_CheckInput(void);

extern volatile uint8_t necTriggerFlag;

#endif // NEC_H_INCLUDED
