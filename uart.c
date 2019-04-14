#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

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

volatile uint8_t uart_str_complete = 0;     // 1 .. String komplett empfangen
volatile uint8_t uart_str_count = 0;
volatile char uart_string[UART_MAXSTRLEN + 1] = "";

void USART_Init(void)
{
    /* Set baud rate */
    UBRRH = (unsigned char)(UBRR_VAL>>8);
    UBRRL = (unsigned char)UBRR_VAL;
    UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0); // Asynchron 8N1
    UCSRB |= (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);  // UART RX, TX und RX Interrupt einschalten
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

ISR(USART_RXC_vect)
{
  unsigned char nextChar;

  // Daten aus dem Puffer lesen
  nextChar = UDR;
  if( uart_str_complete == 0 ) {	// wenn uart_string gerade in Verwendung, neues Zeichen verwerfen

    // Daten werden erst in uart_string geschrieben, wenn nicht String-Ende/max Zeichenlänge erreicht ist/string gerade verarbeitet wird
    if( nextChar != '\n' &&
        nextChar != '\r' &&
        uart_str_count < UART_MAXSTRLEN ) {
      uart_string[uart_str_count] = nextChar;
      uart_str_count++;
    }
    else {
      uart_string[uart_str_count] = '\0';
      uart_str_count = 0;
      uart_str_complete = 1;
    }
  }
}
