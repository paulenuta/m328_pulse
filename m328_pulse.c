// Emits pulses on PB0 using the CTC timer interrupt
//
#include <avr/io.h>
#include <avr/interrupt.h>

// Timer 0 prescaling - divides by 1024=101, 256=100, 64=011, 8=010, 1=001
#define START_TIMER1 TCCR1B |= ((1 << WGM12)|(1<<CS12)|(1<<CS11)|(1<<CS10))
#define STOP_TIMER1  TCCR1B &= ((0 << WGM12)|(0<<CS12)|(0<<CS11)|(0<<CS10))
#define CLEAR_TIMER1 TCNT1   = 0

volatile unsigned char contor = 0;
volatile unsigned char pulseCount = 10;
unsigned char read;

static inline void  SystemInit(void)
{
	cli();										// Deactivate global interrupts
	//MCUCR |= ((1<<ISC01) |(0<<ISC00));			// INT0 is falling edge
	//PCMSK |= ((1<<PCINT3)|(0<<PCINT4));		// Pin Change Enable Mask PB2 or PB3
	//GIMSK |= ((1<<PCIE)  |(1<<INT0));			// Enable external interrupts INT0 & PCINT
	
	TCCR1A |= ((0 << COM1A1) | (1 << COM1A0)); //Toggle OC0A/OC0B on Compare Match
	OCR1A = 2; 								// Cycles for interrupt @50ms @4.8MHz
	TIMSK1 |= (1 << OCIE1A);					// Enable timer compare interrupt
	sei();										// Activate global interrupts
}
 
int main(void)
{
	DDRB |= ((1 << PD6)|(1 << PB5));						// Set OC0A as an output
	SystemInit();
	START_TIMER1;
	while(1) { }								// Don't do anything in main
}

ISR(TIMER1_COMPA_vect)							// Interrupt Service Routine
{
	//contor++;
	PORTB ^= _BV(PB5);
if (contor == pulseCount)						//Stops the timer on specified no of pulses
   {
	STOP_TIMER1;
	contor = 0;
	pulseCount = 0;
   }
   return;
}

ISR (INT0_vect)									// Interrupt Service Routine INT0
{
	pulseCount = 6;
	CLEAR_TIMER1;
	START_TIMER1;
	return;
}

ISR(PCINT0_vect)								// Interrupt Service Routine PCINT0
{
	read  = PIN3;
if (bit_is_clear(read,3))						// This is falling edge
   {
	pulseCount = 2;
	CLEAR_TIMER1;
	START_TIMER1;
   }
	return;
}

