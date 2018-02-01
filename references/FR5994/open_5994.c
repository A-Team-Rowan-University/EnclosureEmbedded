

#include <msp430.h>

//Function Setup//
void TimerAInit(void);
void TimerBInit(void);
void ADC12Init(void);
void UARTInit(void);
void Fan_GPIOInit(void);
void Temp_GPIOInit(void);

//Variable declaration//
unsigned volatile int		adc_in = 0;
volatile float			tempC = 0;
volatile float			tempC_set = 0;
volatile float			tempF = 0;
volatile float			voltage = 0; 
int				PWM = 0;

void Temp_GPIOInit()
{
	P1OUT &= ~BIT0;        // Clear LED to start
	P1DIR |= BIT0;         // P1.0 output
	P1SEL1 |= BIT5;        // Configure P1.5 for ADC
	P1SEL0 |= BIT5;
}
void Fan_GPIOInit(void)
{
	//For pin 1.4
	P1DIR |= BIT4;		//Pin 1.4
	P1SEL1 &= ~BIT4;	//control which functions will be connected or multiplexed onto the pins.  
	P1SEL0 |= BIT4;		//The higher four bits have as their function to enable JTAG or to disable it.
}

int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;		// Stop WDT
	PM5CTL0 &= ~LOCKLPM5;			//Disable HIGH Z mode
	Fan_GPIOInit();					//Fan Pin Initialization
	Temp_GPIOInit();				//Temperature GPIO Initialization
	TimerAInit();					//Timer A Function call UART
	TimerBInit();					//Timer B Function call PWM
	UARTInit();						//UART Initialization


	while (REFCTL0 & REFGENBUSY);            // If ref generator busy, WAIT
	REFCTL0 |= REFVSEL_0 + REFON;           // Enable internal 1.2 reference

	ADC12Init();				//ADC12 Function call

	while (!(REFCTL0 & REFGENRDY));          // Wait for reference generator
	__enable_interrupt(); //Enable interrupts.

	while (1)
	{

		__bis_SR_register(LPM0 + GIE); // Enter LPM0, interrupts enabled
		__no_operation(); // For debugger
	}
}
//ADC ISR
#pragma vector=EUSCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
	switch (__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
	{
	case USCI_NONE: break;
	case USCI_UART_UCRXIFG:
		while (!(UCA0IFG&UCTXIFG));

		tempC_set = UCA0RXBUF;             // send RX to tempC_set

		if (tempC_set <= 32)
		{
			PWM = 0xFF;
		}
		else if (tempC_set > 32 && tempC_set <= 34)
		{
			PWM = ((tempC_set - 36.87) / -0.0917)
		}
		else if (tempC_set > 34 && tempC_set <= 36)
		{
			PWM = ((tempC_set - 48.24) / -0.08)
		else if (tempC_set > 36 && tempC_set <= 39)
			{
				PWM = ((tempC_set - 42.03) / -0.0394)
			}
		else if (tempC_set > 39 && tempC_set <= 43)
		{
			PWM = ((tempC_set - 50.85) / -0.1538)
		}
		else if (tempC_set > 43 && tempC_set <= 54)
		{
			PWM = ((tempC_set - 57.90) / -0.2759)
		}
		else if (tempC_set > 54)
		{
			PWM = 25;
		}

		TB0CCR1 = PWM;

		break;

	case USCI_UART_UCTXIFG: break;
	case USCI_UART_UCSTTIFG: break;
	case USCI_UART_UCTXCPTIFG: break;

	default:
		break;
	}
}

void UARTInit(void)
{

	CSCTL0_H = CSKEY_H;                         // Unlock CS registers
	CSCTL1 = DCOFSEL_3 | DCORSEL;               // Set DCO to 8MHz
	CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
	CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;       // Set all dividers
	CSCTL0_H = 0;                               // Lock CS registers

	P2SEL0 &= ~(BIT0 | BIT1);                   //Configure pin 2.0 to RXD
	P2SEL1 |= BIT0 + BIT1;                      //Configure pin 2.1 to TXD
												// Configure USCI_A0 for UART mode
	UCA0CTLW0 = UCSWRST;                        // Put eUSCI in reset
	UCA0CTLW0 |= UCSSEL__SMCLK;                 // CLK = SMCLK
	UCA0BRW = 52;                               // 8000000/16/9600
	UCA0MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
	UCA0CTLW0 &= ~UCSWRST;                      // Initialize eUSCI
	UCA0IE |= UCRXIE;                           // Enable USCI_A0 RX interrupt
}
void ADC12Init(void)
{
	ADC12CTL0 = ADC12SHT0_2 + ADC12ON;      // Set sample time
	ADC12CTL1 = ADC12SHP;                   // Enable sample timer
	ADC12CTL2 |= ADC12RES_2;                // 12-bit conversion results
	ADC12MCTL0 = ADC12INCH_5 | ADC12VRSEL_1;// Vref+ = , Input
	ADC12IER0 |= ADC12IE0;                  // Enable ADC conv complete interrupt

}

void TimerAInit(void)  //Timer used for UART
{
	TA0CCTL0 = CCIE;			//Disable timer Interrupt
	TA0CCTL1 = OUTMOD_3;			//Set/Reset when the timer counts to the TA0CCR1 value, reset for TA0CCR0
	TA0CCR1 = 256;
	TA0CCR0 = 4096 - 1;			//Set CCR0 for a ~1kHz clock.
	TA0CTL = TASSEL_1 + MC_1 + ID_3;	//Enable Timer A with SMCLK
}
void TimerBInit(void) //PWM Timer
{
	TB0CCTL1 = OUTMOD_3;			//Set OUTMOD_3 (set/reset) for CCR1
									//Set initial values for CCR1 (255 -> 254)
	TB0CCR1 = 0xFF;				//reset and set immediately (May change to slower clock)
	TB0CCR0 = 255 - 1;			//Set CCR0 for a ~1kHz clock.
	TB0CTL = TBSSEL_2 + MC_1;		//Enable Timer B0 with SMCLK and up mode. 1MHz
}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
	ADC12CTL0 |= ADC12SC | ADC12ENC;	//start ADC conversation
}
//ADC ISR
#pragma vector=ADC12_B_VECTOR
__interrupt void ADC12ISR(void)
{
	adc_in = ADC12MEM0;		    //set ADC12MEM to variable
	voltage = adc_in * 0.00029;	    //converts ADC to voltage
	tempC = voltage / 0.01;		     //converts voltage to Temp C
	tempF = ((9 * tempC) / 5) + 32;     //Temp C to Temp F
	while (!(UCA0IFG&UCTXIFG));
	UCA0TXBUF = tempC;		     //change to =tempF to output in Fahrenheit 
}
