
#ifndef F_CPU
#define F_CPU 10000000 // 16 MHz clock speed
#endif

#include <avr/interrupt.h>

#include <avr/io.h>
#include <util/delay.h>

#include "I2C.h"


static inline void initInterrupt(void)
{
	SREG |=(1<<7);
	
	MCUCR |= (0<<ISC01)|(1<<ISC00);
	GIMSK |= (1 << PCIE0)|(0 << PCIE1)|(0<<INT0);   // pin change interrupt enable
	
	DDRA &= ~(1<<PB2);
	DDRA &= ~(1<<PA3);
	
	PCMSK0 = (1<<PCINT2)|(1<<PCINT3);
	
	PCMSK1 = 0;//(1<<PCINT8)|(1<<PCINT9);


	//PCMSK0 |= (1 << PCINT0)|(1 << PCINT1);
	// enable interrupts
}


#define true 1
#define false 0
#define high 1
#define low 0

int direction = 0;

int pin0_state0= 0;
int pin0_state1= 0;
int pin1_state0= 0;
int pin1_state1= 0;

int pin1_count = 0;
int pin0_count = 0;

ISR(PCINT0_vect)
{
	pin0_state0 = pin0_state1;
	pin0_state1= (PINA & (1<<PA2));

	pin1_state0 = pin1_state1;
	pin1_state1= (PINA & (1<<PA3));
	
	//direction
	if(pin0_state0 == pin1_state0 && pin0_state1 != pin1_state1 && pin1_state0 != pin1_state1)
	direction = 1;
	else if(pin0_state0 == pin1_state0 && pin0_state1 != pin1_state1 && pin0_state0 != pin0_state1)
	direction = -1;
	
	//pin count
	if(pin0_state0 != pin0_state1)
	pin0_count+=direction;
	if(pin1_state0 != pin1_state1)
	pin1_count+=direction;
	
	DDRA |= (1<<PA0);
	PORTA |= (1 << PA0);
	
	//int deltaTime = TCNT1;
	//TCNT1 = 0;
	//TIFR |= (1<<OCF1A) ; //clear timer1 overflow flag
}

//PORTD |= (1 << PORTD3);
//PORTD &= ~(1 << PORTD3);


void configurePWM_timer0()
{
	//pwm configuration by using timer0
	
	//set pwm pins as output pins
	DDRB = (1<<PB2);
	DDRA = (1<<PA7);
	
	//set pwm duty cycle registers to 0
	OCR0A = 0;
	OCR0B = 0;
	
	//clear timer configuraiton registers
	//TCCR0A=0;
	//TCCR0B =0;
	
	//set clear on compare match
	TCCR0A |= (1<<COM0A1)|(0<<COM0A0);
	TCCR0A |= (1<<COM0B1)|(0<<COM0B0);
	
	//page 75: table 13-8: pwm phase correct mode
	TCCR0B |= (0<<WGM02);
	TCCR0A |= (0<<WGM01)|(1<<WGM00);
	
	//these are set to zero to just in case
	TCCR0B &= ~(0<<FOC0A);
	TCCR0B &= ~(0<<FOC0B);
	
	//page 76: timer set to no prescaling, we want highest frequency
	TCCR0B |= (0<<CS02)|(0<<CS01)|(1<<CS00);
	
	//OCR0B and OCR0A are used to set the duty cycle of pwm pins
}

void setPWM_OC0A(uint8_t dutyCycle)
{
	OCR0A = dutyCycle;
}

void setPWM_OC0B(uint8_t dutyCycle)
{
	OCR0B = dutyCycle;
}


void setMotor_direction(uint8_t throttle, uint8_t direction)
{
	if(direction == 1)
	{
		setPWM_OC0A(throttle);
		setPWM_OC0B(0);
	}
	else
	{
		setPWM_OC0A(throttle);
		setPWM_OC0B(0);
	}
}

void setMotor(int throttle)
{
	if(throttle > 0)
	{
		if(throttle>255)
		throttle=255;
		
		setPWM_OC0A(throttle);
		setPWM_OC0B(0);
	}
	else if(throttle < 0)
	{
		throttle *= -1;
		
		if(throttle>255)
		throttle=255;
		
		setPWM_OC0A(0);
		setPWM_OC0B(throttle);
	}
	else
	{
		setPWM_OC0A(0);
		setPWM_OC0B(0);
	}
}


int main(void)
{
	configurePWM_timer0();
	
	int inc = 0;

	USI_I2C_Init(0x2);

	initInterrupt();
	sei();

	static long int destination = 0;
	uint8_t dir = 0;
	
	int tt=1;
	float yy=0;
	while(1)
	{
		long int pwm;
		long int r = 11*600*2;
		
		//if(receiveBuffer.size >0)
		//	destination = 100*(long int)(dequeue(&receiveBuffer));
		//else
		
		if(destination == 100000)
		{
		tt=-1;
		}
		if(destination == -100000)
		{
			tt=1;
		}
		destination += tt; 
		
		
		inc = (inc+1)%2;
		
		
		static long int prevError = 0;
		static long int error = 0;
		
		prevError = error;
		error = pin1_count-destination/100;
		
		
		static float integral = 0;
		integral += error;
		
		int m = 2000;
		if(integral>m)integral=m;
		if(integral<-m)integral=-m;
		
		long int res  = error*6 + integral/50 - 2*(error - prevError);
		
		pwm = res;
		int pwmmax = 255;
		
		if(pwm>pwmmax)
		pwm=pwmmax;
		
		if(pwm<-pwmmax)
		pwm=-pwmmax;
		
		setMotor(pwm);
	}
	
	return 0;
}


