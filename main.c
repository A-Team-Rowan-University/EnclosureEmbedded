//*****************************************//
// Heat Enclosure with the MSP430G2553 1.0 //
// Steven Portley and Jack Pedicone		   //
// Last updated 2/6/2018				   //			
//*****************************************//

// SUBJECT TO CHANGE

#include <msp430g2553.h>

//Function Setup//
void TimerAInit(void);
void TimerBInit(void);
void ADC10Init(void);
void fanControl(void);
void tempInit(void);
void tempControl(void);

//Variable declaration//
unsigned volatile int		adc_in 		= 0;
volatile float				tempC 		= 0;
volatile float				tempC_set 	= 0;
volatile float				tempF 		= 0;
volatile float				voltage 	= 0;

void tempInit()
{
	//temperature probe needs to be configured for the ADC pin of the G2
	P1OUT &= ~BIT0;        // Clear LED to start
	P1DIR |= BIT0;         // P1.0 output
	P1SEL1 |= BIT1;        // Configure P1.1 for ADC
	P1SEL0 |= BIT1;
}

void tempControl()
{
	
}

void fanControl(void)
{
	//For pin 1.4
	P1DIR |= BIT4;		//Pin 1.4
	P1SEL1 &= ~BIT4;	//control which functions will be connected or multiplexed onto the pins.  
	P1SEL0 |= BIT4;		//The higher four bits have as their function to enable JTAG or to disable it.
}

void ADC10Init(void)
{
	ADC10CTL0 = ADC10SHT0_2 + ADC10ON;      // Set sample time for the ADC10 control register 0, turn ADC10 on
	ADC10CTL1 = ADC10SHP;                   // Enable sample timer for the ADC10 control register 1
	ADC10CTL2 |= ADC10RES_2;                // 10-bit conversion results
	ADC10MCTL0 = ADC10INCH_5 | ADC10VRSEL_1;// Vref+ = , Input
	ADC10IER0 |= ADC10IE0;                  // Enable ADC conv complete interrupt
}

void TimerAInit(void)  //Timer used the outside probe
{
	TA0CCTL0 = CCIE;					//Disable timer Interrupt
	TA0CCTL1 = OUTMOD_3;				//Set/Reset when the timer counts to the TA0CCR1 value, reset for TA0CCR0
	TA0CCR1 = 256;
	TA0CCR0 = 4096 - 1;					//Set CCR0 for a ~1kHz clock.
	TA0CTL = TASSEL_1 + MC_1 + ID_3;	//Enable Timer A with SMCLK
}

int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;		// Stop WDT
	fanControl();					// Fan Pin Initialization
	tempInit();				// Temperature GPIO Initialization
	TimerAInit();					// Timer A Function call UART
	TimerBInit();					// Timer B Function call PWM

	while (REFCTL0 & REFGENBUSY);            // If ref generator busy, WAIT
	REFCTL0 |= REFVSEL_0 + REFON;           // Enable internal 1.2 reference

	ADC10Init();				//ADC10 Function call

	while (!(REFCTL0 & REFGENRDY));          // Wait for reference generator
	__enable_interrupt(); //Enable interrupts.

	while (1)
	{
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
#pragma vector=ADC12_B_VECTOR
__interrupt void ADC10ISR(void)
{
	adc_in = ADC10MEM0;		    //set ADC12MEM to variable
	voltage = adc_in * 0.00029;	    //converts ADC to voltage
	tempC = voltage / 0.01;		     //converts voltage to Temp C
	tempF = ((9 * tempC) / 5) + 32;     //Temp C to Temp F
	while (!(UCA0IFG&UCTXIFG));
	UCA0TXBUF = tempC;		     //change to =tempF to output in Fahrenheit 
}


