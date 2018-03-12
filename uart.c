#include "uart.h"
#include <avr/io.h>

#ifndef F_CPU
#warning "F_CPU war noch nicht definiert, wird nun nachgeholt mit 4000000"
#define F_CPU 16000000UL  // Systemtakt in Hz - Definition als unsigned long beachten
                         // Ohne ergeben sich unten Fehler in der Berechnung
#endif

#define BAUD 19200UL      // Baudrate

// Berechnungen
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   // clever runden
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))     // Reale Baudrate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD) // Fehler in Promille, 1000 = kein Fehler.

#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
  #error Systematischer Fehler der Baudrate grösser 1% und damit zu hoch!
#endif

void USART_Init(void)
{
/* Set baud rate */
UBRRH = (unsigned char)(UBRR_VAL>>8);
UBRRL = (unsigned char)UBRR_VAL;
/* Enable receiver and transmitter */
UCSRB = (1<<RXEN)|(1<<TXEN);
/* Set frame format: 8data, 2stop bit */
UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
}

void USART_putc(unsigned char data)
{
/* Wait for empty transmit buffer */
while ( !( UCSRA & (1<<UDRE)) )
;
/* Put data into buffer, sends the data */
UDR = data;
}

/* puts ist unabhaengig vom Controllertyp */
void USART_puts (char *s)
{
    while (*s)
    {   /* so lange *s != '\0' also ungleich dem "String-Endezeichen(Terminator)" */
        USART_putc(*s);
        s++;
    }
}
