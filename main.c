//*****************************************//
// Heat Enclosure with the MSP430G2553 	   //
// Jack Pedicone		   				   //
// Last updated 2/9/2018				   //			
//*****************************************//

// SUBJECT TO CHANGE

#include <msp430g2553.h>

//Function Setup//
void TimerAInit(void);
void TimerBInit(void);
void ADC10Init(void);
void fanInit(void);
void fanControl(void);
void tempInit(void);
void tempControl(void);
void tempSwitch(void);

//Variable declaration//
unsigned volatile int		adc_in 		= 0;
volatile float				tempC 		= 0;
volatile float				voltage 	= 0;

volatile float				tempO		= 0;
volatile float				tempI		= 0;

void tempInit()
{
	//sensor 1
	P2DIR |= BIT0; //set pin 2.0 and 2.1 as outputs
	P2DIR |= BIT1;
	
	P2OUT |= BIT0;
}

void tempControl(void)
{
	
}

void tempSwitch(void)
{
	P2OUT ^= BIT0;
	P2OUT ^= BIT1;
}

void fanInit(void)
{
	P1DIR |= BIT1;		//Pin 1.1
	P1SEL1 &= ~BIT1;	//control which functions will be connected or multiplexed onto the pins.  
	P1SEL0 |= BIT1;		//The higher four bits have as their function to enable JTAG or to disable it.
}

void ADC10Init(void)
{
	ADC10CTL0 = ADC10IE + ADC10SHT0_2 + ADC10ON + MSC;      // Set sample time for the ADC10 control register 0, turn ADC10 on
	ADC10CTL1 = ADC10SHP + ADC10INCH_3 + ADC10SSEL_0 + CONSEQ_2;                   // Enable sample timer for the ADC10 control register 1
	ADC10AE0 |= BIT3; 
}

void TimerAInit(void)  //Timer used the temperature sensor
{
	TA0CCTL0 = CCIE;					//Disable timer Interrupt
	TA0CCTL1 = OUTMOD_3;				//Set/Reset when the timer counts to the TA0CCR1 value, reset for TA0CCR0
	TA0CCR1 = 256;
	TA0CCR0 = 4096 - 1;					//Set CCR0 for a ~1kHz clock.
	TA0CTL = TASSEL_1 + MC_1 + ID_3;	//Enable Timer A with SMCLK
}

void TimerBInit(void)  //Timer used for the fan
{
	TB0CCTL1 = OUTMOD_3;			//Set OUTMOD_3 (set/reset) for CCR1
									//Set initial values for CCR1 (255 -> 254)
	TB0CCR1 = 0xFF;				    //reset and set immediately (May change to slower clock)
	TB0CCR0 = 255 - 1;			    //Set CCR0 for a ~1kHz clock.
	TB0CTL = TBSSEL_2 + MC_1;		//Enable Timer B0 with SMCLK and up mode. 1MHz
}

int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;		// Stop WDT
	fanControl();					// Fan Pin Initialization
	tempInit();						// Temperature GPIO Initialization
	TimerAInit();					
	TimerBInit();					

	while (REFCTL0 & REFGENBUSY);            // If ref generator busy, WAIT
	REFCTL0 |= REFVSEL_0 + REFON;           // Enable internal 1.2 reference

	ADC10Init();				//ADC10 Function call

	while (!(REFCTL0 & REFGENRDY));          // Wait for reference generator
	__enable_interrupt(); //Enable interrupts.

	while (1)
	{
		if (!(tempO == 0) && !(tempI == 0))
		{
			tempControl();
		}
		__bis_SR_register(LPM0 + GIE); // Enter LPM0, interrupts enabled
		__no_operation(); // For debugger
	}
}



#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
	ADC10CTL0 |= ADC10SC | ADC10ENC;	//start ADC conversation
}


//ADC ISR
#pragma vector=ADC10_B_VECTOR
__interrupt void ADC10ISR(void)
{
	adc_in = ADC10MEM0;		    //set ADC10MEM to variable
	
	voltage = adc_in * 0.00029;	    //converts ADC to voltage
	tempC = voltage / 0.01;		     //converts voltage to Temp C
	
	switch (P2OUT)
	case 
	if (P2OUT == BIT0)
	{
		tempO = tempC;
		P2OUT &= ~BIT0;
		P2OUT |= BIT1;
	}
	else
	if (P2OUT == BIT1)
	{
		tempI = tempC;
		P2OUT &= ~BIT1;
		P2OUT |= BIT0;
	}
}


