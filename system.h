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

typedef enum
{
    MAIN_STATE_POWER_DOWN       = 0,
    MAIN_STATE_PLAY_DEMO        = 1,
    MAIN_STATE_MONOCHROME_CUBE  = 2,
    MAIN_STATE_RGB_FADING       = 3
}mainState_t;

void InitADC(void);
void InitRandomGenerator(void);
void intToASCII(uint8_t src, char* dest);
void InitLayerPins(void);

#endif // SYSTEM_H_INCLUDED
