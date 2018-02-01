//******************************************************************************
//
//                    MSP430G2553
//                -------------------
//             /|\  |          VCC|- 3.3V
//              |   |                 |
//          --  |GND          P1.1|--< RX
//              |         P1.2|--> TX
//              |         |
//                  |                 |
//         adc_value <--|P1.7/A1      P1.0|--> LED
//                -------------------
//  adc_value (P1.7) to LM35
//
//  Filename : msp430g2553-ADC10.c
//
//  Created on : October 22, 2017
//  Updated on : November 15, 2017
//  Author : Joshua Gould
//
//******************************************************************************
/* --COPYRIGHT--,BSD_EX
* Copyright (c) 2014, Texas Instruments Incorporated
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* *  Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* *  Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* *  Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************
*
*                       MSP430 CODE EXAMPLE DISCLAIMER
*
* MSP430 code examples are self-contained low-level programs that typically
* demonstrate a single peripheral function or device feature in a highly
* concise manner. For this the code may rely on the device's power-on default
* register values and settings such as the clock configuration and care must
* be taken when combining code from several examples to avoid potential side
* effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
* for an API functional library-approach to peripheral configuration.
*
* --/COPYRIGHT--*/

//LIBRARY INIT & DEFINITIONS///////////////////////////////////////

#include <msp430.h>
void initTimer(void);
void initADC10(void);
void initUART(void);
void initClock(void);

unsigned int adc_value = 0;
float tempC = 0;
float tempF = 0;
float voltage = 0;
int pwm = 0;

//LIBRARY INIT & DEFINITIONS///////////////////////////////////////

//MAIN FUNCTION////////////////////////////////////////////////////
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

	initADC10();  //Call Functions
	initTimer();
	initUART();
	initClock();

	__bis_SR_register(GIE);       //interrupt enable
	while (1) {
	}
}

//MAIN FUNCTION////////////////////////////////////////////////////

//////////////////////////////////////////////////
/*                      */
/*      INITIALIZATIONS         */
/*                      */
//////////////////////////////////////////////////

//ADC INITIALIZATION///////////////////////////////////////////////

void initADC10()
{
	ADC10CTL1 = INCH_7 + SHS_1;            // P1.7, TA1 trigger sample start
	ADC10AE0 = BIT7;                       // ADC10 on P1.7
	P1DIR |= BIT0;                         // set LED1 to output
}
//ADC INITIALIZATION///////////////////////////////////////////////

//TIMER INITIALIZATION/////////////////////////////////////////////
void initTimer()
{
	P2SEL |= BIT1;				//Pwm at P2.1
	TA1CCTL1 = OUTMOD_7;		//PWM output mode: 7 - PWM reset/set
	TA1CCR0 = 7;				//for max voltage.max voltage diveded seven part
	TA1CCR1 = 0;				//for min voltage
	TA1CTL = TASSEL_2 + MC_1;				//Timer A clock source select: 2 - SMCLK,up mode
}
//TIMER INITIALIZATION/////////////////////////////////////////////

//UART INITIALIZATION//////////////////////////////////////////////
void initUART()
{
	if (CALBC1_1MHZ == 0xFF)                // If calibration constant erased
	{
		while (1);
	}

	DCOCTL = 0;                             // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ;                  // Set DCO
	DCOCTL = CALDCO_1MHZ;
	P1SEL = BIT1 + BIT2;                    // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2;                   // P1.1 = RXD, P1.2=TXD
	UCA0CTL1 |= UCSSEL_2;                   // SMCLK
	UCA0BR0 = 104;                          // 1MHz 9600
	UCA0BR1 = 0;                            // 1MHz 9600
	UCA0MCTL = UCBRS0;                      // Modulation UCBRSx = 1
	UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
	IE2 |= UCA0RXIE;                        // Enable USCI_A0 RX interrupt
}
//UART INITIALIZATION//////////////////////////////////////////////

//CLOCK INITIALIZATION/////////////////////////////////////////////
void initClock()
{
	DCOCTL = 0;                             // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ;                  // Set DCO
	DCOCTL = CALDCO_1MHZ;
}
//CLOCK INITALIZATION//////////////////////////////////////////////

//////////////////////////////////////////////////
/*                      */
/*      INTERRUPTS          */
/*                      */
//////////////////////////////////////////////////

//ADC INTERRUPT////////////////////////////////////////////////////
//Reference from Jessica Wozniak for temp conversion
//Takes value from ADC and converts it to a meaningful value
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
	adc_value = ADC10MEM;
	voltage = adc_value* 0.0033;          //Converts ADC to voltage. (Vref/2^10) = 0.0033 * ADC = voltage
	tempC = voltage / 0.01;               //For LM35 each degree C is 10mv (0.01V)
	tempF = ((9 * (tempC)) / 5) + 32;             //converts degrees C to degrees F

												  //adjust and write motor speed depends on tempCerature
	if (tempC >= 0 && tempC <= 20) {			//if tempC between 0 and 20 adjust fanspeed to 1/7th of normal value

		TA1CCR0 = 1;
		__delay_cycles(65000);
	}

	else if (tempC >= 21 && tempC <= 25) {
		TA1CCR0 = 2;
		__delay_cycles(65000);
	}

	else if (tempC >= 26 && tempC <= 30) {
		TA1CCR0 = 3;

		__delay_cycles(65000);
	}

	else if (tempC >= 31 && tempC <= 35) {
		TA1CCR0 = 5;
		__delay_cycles(65000);
	}

	else if (tempC >= 36 && tempC <= 40) {
		TA1CCR0 = 5;
		__delay_cycles(65000);
	}

	else if (tempC >= 41 && tempC <= 45) {
		TA1CCR0 = 6;
		__delay_cycles(65000);

	}

	else {
		TA1CCR0 = 7;
		__delay_cycles(65000);
	}

	while (!(IFG2 & UCA0TXIFG));          //waits for tx to be clear
	UCA0TXBUF = tempC;                   //send to TX in fahrenheit  (write tempC for Celcius)
}
//ADC INTERRUPT////////////////////////////////////////////////////

//TIMER INTERRUPT//////////////////////////////////////////////////
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)

{
	ADC10CTL0 = SREF_0 + ADC10SHT_2 + REFON + ADC10ON + ADC10IE;    //AVSS, 16 x ADC10CLKs, ADC10 Reference on, ADC10 On/Enable, ADC10 Interrupt Enable
	ADC10CTL0 |= ENC;                                               // ADC10 enable set
}
//TIMER INTERRUPT//////////////////////////////////////////////////

//UART INTERRUPT

//  Echo back RXed character, confirm TX buffer is ready first
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR(void)
#else
#error Compiler not supported!
#endif
{
	while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?

											  //set input start value S = 83
	if (UCA0TXBUF = 83) {
		UCA0TXBUF = ADC10MEM; //adds ADC value and skips clockcycle apparently
		P1OUT ^= 0x01;
	}
	else {
		UCA0TXBUF = 0;       //else code to print stop perhaps
	}

}