/*
 * Includes system specific functions like pin assignment
 */
 #include "system.h"
 #include "uart.h"
 #include <stdlib.h>

void InitADC(void) {
    ADMUX |= (1<<REFS0)|(1<<MUX2);  // Set Reference to AVCC and input to ADC4

    ADCSRA |= (1<<ADEN)|(1<<ADPS0)  // Enable ADC, set prescaler to 128
             |(1<<ADPS1)|(1<<ADPS2);// Fadc=Fcpu/prescaler=16000000/128=125kHz
                                    // Fadc should be between 50kHz and 200kHz
}

void InitRandomGenerator(void) {
    uint8_t randSeed = 0;
    for(uint8_t i = 0; i < 8; i++) {
        ADCSRA |= (1<<ADSC);        // Start a conversion
        while(ADCSRA & (1<<ADSC));  // What until the bit is reset by the
                                    // CPU after the conversion is finished
        randSeed <<= 1;
        randSeed |= (ADC & 0x1);
    }

    srand(randSeed);
}

void intToASCII(uint8_t src, char* dest)
{
    uint8_t h_src = 0;

    h_src = src/100;
    dest[0] = h_src + 48;

    src -= h_src * 100;
    h_src = src/10;
    dest[1] = h_src + 48;

    src -= h_src * 10;
    dest[2] = src + 48;
}

void InitLayerPins(void)
{
    DDRC |= (1 << LAYER_1_PIN) | (1 << LAYER_2_PIN) | (1 << LAYER_3_PIN) | (1 << LAYER_4_PIN);
    PORTC &= ~((1 << LAYER_1_PIN) | (1 << LAYER_2_PIN) | (1 << LAYER_3_PIN) | (1 << LAYER_4_PIN));
}
