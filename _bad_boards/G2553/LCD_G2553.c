//******************************************************************************
// MSP430G2553 - ADC, Low Power Mode, TIMER
//
// Description: program read analog value(temperature) from P1.6 and
// write to lcd display(16*2). Also when temperature increase, motor(at P2.1)
// speed increase too.Analog value read once time at 0.5msec.msp is at low power mode
// as timer flag set.
//
// Ali Ýhsan Doydu
// Hacettepe University
// December 2010
//******************************************************************************
/*
* Joshua Gould
* MSP430G2553 - Visualizing Data LCD
* Created: 11 - 07 - 17
* Updated 11 - 22 -2017
*/
#include <msp430.h>
#include "lcd_4bit.h"
//required global variables
int adc_result = 0;
int temp = 0;
unsigned char dizi[2];
int x = 1;
int i;
//for read analog value
void adc_init(void) {
	ADC10CTL0 &= ~ENC;												//ADC10 enable conversion,ADC10 disabled
	ADC10CTL0 = SREF_1 + REFON + ADC10SHT_3 + ADC10ON + ADC10IE;	//1.5v reference voltage, 64*ADC10 clocks, ADC10 on, interrupt enable
	ADC10CTL1 = INCH_6 + ADC10SSEL_3 + ADC10DIV_3;					//chosen internal temperature sensor,SMCLK,clock divider 3
	ADC10AE0 = BIT6;												//Anlog enable for internal temperature PA.6
}
int main(void) {
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
	DCOCTL = CALDCO_1MHZ;
	BCSCTL1 = CALBC1_1MHZ;
	
	//required ADC adjustment
	P1SEL = 0x00;
	P1OUT = 0x00;
	P1DIR = 0xFF;		//All bits as out
	P2SEL = 0x00;
	P2DIR = 0xFF;		//All bits as out
	
	// UART Stuff
	DCOCTL = 0;                               // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
	DCOCTL = CALDCO_1MHZ;
	P1SEL = BIT1 + BIT2;                     // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2;                    // P1.1 = RXD, P1.2=TXD			May be repeat
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	UCA0BR0 = 104;                            // 1MHz 9600
	UCA0BR1 = 0;                              // 1MHz 9600
	UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
	//UART End

	//required pwm adjustment
	P2SEL |= BIT1;				//Pwm at P2.1
	TA1CCTL1 = OUTMOD_7;		//PWM output mode: 7 - PWM reset/set
	TA1CCR0 = 7;				//for max voltage.max voltage diveded seven part
	TA1CCR1 = 0;				//for min voltage
	TA1CTL = TASSEL_2 + MC_1;				//Timer A clock source select: 2 - SMCLK,up mode
							 //required timer adjustment
	CCTL0 = CCIE;
	CCR0 = 50000;//timer count as 50000
	TACTL = TASSEL_2 + MC_1 + TAIE;  //1mikro*50000=50 ms(interrupt run once at 50ms)
									 //call ADC and Lcd adjustments
	adc_init();

	for (;;) {
		_BIS_SR(CPUOFF + GIE);//low power mode
	}
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void bekle(void) {

	ADC10CTL0 |= ADC10SC + ENC; //start conversion and enable conversion
	while (!(ADC10CTL0 & ADC10IFG));// ADC10IFG = 00000100, wait until flag unset
	adc_result = ADC10MEM;//read analog value
	temp = (unsigned int)(adc_result*0.146484375);//convert to degree from analog value.(Vref*1000)/(1024*10),(1C=10mV)
	TACTL = TACTL & (~TAIFG);// flag is unset

							 // _BIC_SR(LPM0_EXIT);//exit low power mode
}

#pragma vector = ADC10_VECTOR
__interrupt void adc_kesmesi(void) {

	x = 1;
	for (i = 0; i <= 1; i++) {
		dizi[i] = ((temp / x) % 10) + 48;
		x = x * 10;
	}
	//adjust and write motor speed depends on temperature
	if (temp >= 0 && temp <= 20) {			//if temp between 0 and 20 adjust fanspeed to 1/7th of normal value
		TA1CCR1 = 1;
		__delay_cycles(65000);
	}

	else if (temp >= 21 && temp <= 25) {
		TA1CCR1 = 2;
		__delay_cycles(65000);
	}

	else if (temp >= 26 && temp <= 30) {
		TA1CCR1 = 3;
		__delay_cycles(65000);
	}

	else if (temp >= 31 && temp <= 35) {
		TA1CCR1 = 5;
		__delay_cycles(65000);
	}

	else if (temp >= 36 && temp <= 40) {
		TA1CCR1 = 5;
		__delay_cycles(65000);
	}

	else if (temp >= 41 && temp <= 45) {
		TA1CCR1 = 6;
		__delay_cycles(65000);
	}

	else {
		TA1CCR1 = 7;
		__delay_cycles(65000);
	}
}