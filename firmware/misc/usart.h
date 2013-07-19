#include <avr/io.h>

void usart_init(uint16_t ubrr);
char usart_getchar( void ); 
void usart_putchar( uint8_t data ); 
void usart_pstr (char *s);