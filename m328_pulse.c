// Emits pulses on PB0 using the CTC timer interrupt
//
#include <avr/io.h>
#include <avr/interrupt.h>

// Timer 0 prescaling - divides by 1024=101, 256=100, 64=011, 8=010, 1=001
#define START_TIMER0 TCCR0B |= ((0 << WGM02)|(1<<CS02)|(1<<CS01)|(0<<CS00))
#define STOP_TIMER0  TCCR0B &= ((0 << WGM02)|(0<<CS02)|(0<<CS01)|(0<<CS00))
#define CLEAR_TIMER0 TCNT0   = 0

volatile unsigned char contor = 0;
volatile unsigned char pulseCount = 10;
unsigned char read;

static inline void  SystemInit(void)
{
	cli();										// Deactivate global interrupts
	//MCUCR |= ((1<<ISC01) |(0<<ISC00));			// INT0 is falling edge
	//PCMSK |= ((1<<PCINT3)|(0<<PCINT4));		// Pin Change Enable Mask PB2 or PB3
	//GIMSK |= ((1<<PCIE)  |(1<<INT0));			// Enable external interrupts INT0 & PCINT
	
	TCCR0A |= ((1 << WGM01)); //Toggle OC0A/OC0B on Compare Match
	OCR0A = 1; 								// Cycles for interrupt @50ms @4.8MHz
	TIMSK0 |= (1 << OCIE0A);					// Enable timer compare interrupt
	sei();										// Activate global interrupts
}
 
int main(void)
{
	DDRB |= ((1 << PD6)|(1 << PB5));						// Set OC0A as an output
	SystemInit();
	START_TIMER0;
	while(1) { }								// Don't do anything in main
}

ISR(TIMER0_COMPA_vect)							// Interrupt Service Routine
{
	//contor++;
	PORTB ^= _BV(PB5);
if (contor == pulseCount)						//Stops the timer on specified no of pulses
   {
	STOP_TIMER0;
	contor = 0;
	pulseCount = 0;
   }
   return;
}

ISR (INT0_vect)									// Interrupt Service Routine INT0
{
	pulseCount = 6;
	CLEAR_TIMER0;
	START_TIMER0;
	return;
}

ISR(PCINT0_vect)								// Interrupt Service Routine PCINT0
{
	read  = PIN3;
if (bit_is_clear(read,3))						// This is falling edge
   {
	pulseCount = 2;
	CLEAR_TIMER0;
	START_TIMER0;
   }
	return;
}

