/********************************************************************************
Includes
********************************************************************************/

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "misc/usart.h"
#include "misc/other.h"
 
/********************************************************************************
Macros and Defines
********************************************************************************/

#define BAUD 57600
#define MYUBRR F_CPU/16/BAUD-1
 
/********************************************************************************
Function Prototypes
********************************************************************************/

uint8_t prosessFrame_command(uint16_t frame);
uint8_t prosessFrame_address(uint16_t frame);

/********************************************************************************

********************************************************************************/

volatile uint16_t 	gframe 	= 0;

/********************************************************************************

********************************************************************************/

FILE uart_io = FDEV_SETUP_STREAM(usart_putchar, usart_getchar, _FDEV_SETUP_RW);
// stdout = &uart_io;

/********************************************************************************
Main
********************************************************************************/
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

int main( void ) {
	
    usart_init ( MYUBRR ); 
	
	PORTD 	|= 1 << PD3;
	DDRB 	|= 1 << 0;
	/********************************************************************************
	16-Bit Timer
	********************************************************************************/
	
	TCCR1B |= 1 << CS11;
	
	/********************************************************************************
	INT0
	********************************************************************************/

	MCUCR 	|= 1 << ISC11;
	GICR 	|= 1 << INT1;
	
	/********************************************************************************

	********************************************************************************/
	
	sei();
	while(1){
		_delay_ms(30);
	}
	
	return 0;
}

ISR(INT1_vect){
	cli();
	stdout = &uart_io;
	// Other shit
	uint16_t 	frame = 0;
	
	frame |= 1 << 0;
	
	uint8_t 	count = 2;

	uint8_t 	step1 = 0;
	uint8_t 	step2 = 0;
	
	uint8_t 	kill1 = 1;
	uint8_t 	kill2 = 1;

	TCNT1 = 0;
	// 1 bit
	while(bit_is_clear(PIND, PD3)){ step1 = 1;  }
	// 2 bit
	while(bit_is_set(PIND, PD3) && step1){
		//if(TCNT1 > (105*16) && TCNT1 < (210*16)){ step2 = 0; }else{ step2 = 0; printf("DEAD ON 001\r\n"); }
		//if(TCNT1 > (210*16) && TCNT1 < (230*16)){ step2 = 1; }else{ step2 = 0; printf("DEAD ON 002\r\n"); }
		if(TCNT1 > (215*16) && TCNT1 < (230*16)){ step2 = 1; step1 = 0; frame |= 1 << 1; }
		if(TCNT1 > (105*16) && TCNT1 < (210*16)){ step2 = 1; step1 = 0; frame |= 0 << 1; }
		
	}
	
	TCNT1 = 0;
	// 3 - 14 bit
	while(step2 && count < 14){
		if(TCNT1 > (160*16) && TCNT1 < (200*16)){
			if(bit_is_clear(PIND, PD3)){ 
				frame |= 0 << count;
				while(bit_is_clear(PIND, PD3));
			}else{		
				frame |= 1 << count; 
				while(bit_is_set(PIND, PD3) && kill2){ if(TCNT1 > (225*16)){ kill2 = 0; step2 = 0; } }
			}
			count++;
			TCNT1 = 0;
		}
	}
	if(count == 14){
		gframe = frame; 
		//printf("buffer %d binary %s \r\n", prosessFrame_command(gframe), byte_to_binary(frame));
		usart_putchar(prosessFrame_command(gframe));
		frame = 0;
		gframe = 0;
	}
	sei();
}

uint8_t prosessFrame_command(uint16_t frame){

		uint8_t ncmd = 0;
		
		if(1 & (frame >> 8)){
		
			ncmd |= (1 & (frame >> 8)) 	<< 6;
			ncmd |= (1 & (frame >> 9)) 	<< 5;
			ncmd |= (1 & (frame >> 10)) << 4;
			ncmd |= (1 & (frame >> 11)) << 3;
			ncmd |= (1 & (frame >> 12)) << 2;
			ncmd |= (1 & (frame >> 13)) << 1;
			ncmd |= (1 & (frame >> 14)) << 0;

		}else{
		
			ncmd |= (1 & (frame >> 13)) << 0;
			ncmd |= (1 & (frame >> 12)) << 1;
			ncmd |= (1 & (frame >> 11)) << 2;
			ncmd |= (1 & (frame >> 10)) << 3;
			ncmd |= (1 & (frame >> 9)) << 4;
		
		}
		
	return ncmd;
}
uint8_t prosessFrame_address(uint16_t frame){

		uint8_t ncmd = 0;
			
		ncmd |= (1 & (frame >> 4)) << 0;
		ncmd |= (1 & (frame >> 5)) << 1;
		ncmd |= (1 & (frame >> 6)) << 2;
		ncmd |= (1 & (frame >> 7)) << 3;
	
	return ncmd;
}