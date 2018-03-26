//*****************************************//
// Heat Enclosure with the MSP430G2553 	   //
// Steven Portley and Jack Pedicone        //
// Last updated 3/8/2018				   //			
//*****************************************//

// The PCB will measure the temperatures inside and outside of the 3D Printed Heated Enclosure.
// Pin 2.0 = Temperature inside, Pin 2.2 = Temperature outside

// Credit for the temperature sensor code and header code goes to Ishan Karve
// http://www.smallbulb.net/2012/238-1-wire-and-msp430
// https://github.com/dsiroky/OneWire/blob/master/onewire.c

// SUBJECT TO CHANGE
#include <msp430g2553.h>
#include "DS18B20.h"
#include "delay.h"

//Function Setup//
void TimerA0Init(void);
void TimerA1Init(void);

void tempFanInit(void);
void tempControl(void);
void readTemp(void);

//Variable declaration//

volatile float				tempO		= 0;
volatile float				tempI		= 0;
volatile float				temp		= 0;

volatile float ti;
volatile float to;


int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;		// Stop WDT
	tempFanInit();					// Temperature sensor and fan GPIO Initialization
	TimerA0Init();
	TA1CCTL1 &= ~CCIFG;
	TA1CCTL1 &= ~CCIE;
	_BIS_SR (CPUOFF + GIE);
	//TimerA1Init();

	while (1){}
}

void tempFanInit()
{
	P2DIR |= BIT0; //set pin 2.0 and 2.2 as outputs
	P2DIR |= BIT2;
	
	P1DIR |= BIT1; //Pin 1.1 turns the fan on, and always keeps it on
}

void tempControl(void)
{
	temp = tempO + 5;
	if (tempI < temp)
	{
		P1OUT |= BIT3;
	}
	else
	{
		P1OUT &= ~BIT3;
	}
}


void TimerA0Init(void)  //Timer used for the temperature sensor
{
    TA0CCTL0 &= ~CCIFG;
	TA0CCTL0 |= CCIE;					//Disable timer Interrupt
	TA0CCTL1 |= OUTMOD_3;				//Set/Reset when the timer counts to the TA0CCR1 value, reset for TA0CCR0
	TA0CCR1 = 256;
	TA0CCR0 = 4096 - 1;					//Set CCR0 for a ~1kHz clock.
	TA0CTL |= TASSEL_1 + MC_1 + ID_3;	//Enable Timer A with SMCLK
}

void TimerA1Init(void)  //Timer used for the fan
{
    TA1CCTL1 &= ~CCIFG;
	TA1CCTL1 |= OUTMOD_3;				//Set OUTMOD_3 (set/reset) for CCR1
										//Set initial values for CCR1 (255 -> 254)
	TA1CCR1 = 0xFF;				    	//reset and set immediately (May change to slower clock)
	TA1CCR0 = 255 - 1;			    	//Set CCR0 for a ~1kHz clock.
	TA1CTL |= TASSEL_2 + MC_1;			//Enable Timer A1 with SMCLK and up mode. 1MHz
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    ti = GetData();
    tempI = ((9 * ti) / 5) + 32;     //Temp C to Temp F
    to = GetData();
    tempO = ((9 * to) / 5) + 32;     //Temp C to Temp F
	TA0CCTL0 &= ~CCIFG;
}


#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    TA1CCTL0 &= ~CCIFG;
}
