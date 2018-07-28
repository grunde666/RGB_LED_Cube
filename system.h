#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED
#include <avr/io.h>

#define LAYER_1_PIN PC4
#define LAYER_2_PIN PC5
#define LAYER_3_PIN PC6
#define LAYER_4_PIN PC7

#define SetLayer1()     PORTC = (1 << LAYER_1_PIN)
#define ClearLayer1()   PORTC &= ~(1 << LAYER_1_PIN)

#define SetLayer2()     PORTC = (1 << LAYER_2_PIN)
#define ClearLayer2()   PORTC &= ~(1 << LAYER_2_PIN)

#define SetLayer3()     PORTC = (1 << LAYER_3_PIN)
#define ClearLayer3()   PORTC &= ~(1 << LAYER_3_PIN)

#define SetLayer4()     PORTC = (1 << LAYER_4_PIN)
#define ClearLayer4()   PORTC &= ~(1 << LAYER_4_PIN)

void intToASCII(uint8_t src, char* dest);
void InitLayerPins(void);

#endif // SYSTEM_H_INCLUDED
